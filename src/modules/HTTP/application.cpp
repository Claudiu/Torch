//
// Copyright (C) 2012 Claudiu Tasca <clawgm [at] gmail.com>, Marius Petcu <marius
// [at] porkholt.org>, Nandor Licker <licker.nandor [at] gmail.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <unistd.h>

#include <magic.h>


#include <torch/http.hpp>
#include <torch/sockets.hpp>
#include <torch/util.hpp>
#include <torch/log.hpp>

using namespace Torch;
using namespace Torch::HTTP;
using namespace Torch::Sockets;

namespace Torch{
    namespace HTTP {
        class Connection //this should parse http requests and send them to a function of Application to dispatch them 
            {
                private:
                Socket * sock;
                Application * app;

                public:
                Socket * getSocket() { return sock; }
                
                Connection(Socket * s, Application * a) : sock(s), app(a) {
                }
                
                void readStuff()
                {
                    uint8_t * b = new uint8_t[512];
                    ssize_t s = sock->read(b, 511);
                    if (!s)
                    {
                        delete[] b;
                        throw StringException("Connection closed");
                    }
                    b[s] = '\0';
                    Request req(std::string((char*)b));
                    delete[] b;

                    Response res(sock);
                    app->dispatchRequest(req, res);
                }
                
                ~Connection() {
                    delete sock;
                }
            };
        }
}

std::string Application::getMimeType(const char * staticFile) {
	std::string rez = "text/plain";

	magic_t magic;
	magic = magic_open(MAGIC_MIME_TYPE);
	magic_load(magic, NULL);
	rez = std::string(magic_file(magic, staticFile));
	magic_close(magic);

	if(rez != "text/plain") {
		if(rez == "text/html")
			rez += ";charset=utf-8"; //TODO: Add more charsets see IANA.org
		return rez;
	} else {
		//plan B :(
		std::string sf_string = staticFile;
		std::string extension = sf_string.substr(sf_string.find_last_of(".") + 1);

		// add more here.... Dirty hack :D
		if(extension == "css")
			return "text/css";
		else if(extension == "js")
			return "application/javascript";
		else
			return rez;
	}

}

void Application::dispatchRequest(const Request & req, Response & res)
{
    if (req.method() == "GET")
    {
        std::map<std::string, callback_func>::iterator i = get_map.find(req.url());
        if (i!=get_map.end()) {
            res.setLocation(req.url());
            res.setHeader("Content-Type", "text/html");
            i->second(req, res);
        }
        else
        {  	
						std::string filepath = staticDir + req.url();
						
						if(filepath.find("/../") != std::string::npos) {
							// LFI detected
							// You can even play with the damn fool by sending him fake /etc/shadow files or other stuff
							Log::inst().error("!! Possible LFI attack detected. !!");
							res.send(403);
							
							return;
						} 

						std::fstream file;
            file.open(filepath.c_str()); // lazy way

            if(file.is_open() == true) 
            {
								res.setHeader("Content-Type", getMimeType(std::string(staticDir + req.url()).c_str()));
								
                std::string str((std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>());
                res.send(str);
								file.close();
            } else {
                Log::inst().error("Got nothing to show for: %s -> sending 404", req.url().c_str());
                res.send(404, ":(");
            }
        }         
		} else if(req.method() == "POST") { // BEST EXAMPLE OF DRY
			std::map<std::string, callback_func>::iterator i = post_map.find(req.url());
			
			if(i!=post_map.end()) {
					res.setLocation(req.url());
					res.setHeader("Content-Type", "text/html");
					i->second(req, res);
			} else {
				Log::inst().error("No POST method for: %s -> sending 404", req.url().c_str());
				res.send(404);
			}
		} else if(req.method() == "PUT") {
			std::map<std::string, callback_func>::iterator i = put_map.find(req.url());
			
			if(i!=put_map.end()) {
					res.setLocation(req.url());
					res.setHeader("Content-Type", "text/html");
					i->second(req, res);
			} else {
				Log::inst().error("No PUT method for: %s -> sending 404", req.url().c_str());
				res.send(404);
			}
		} else if(req.method() == "DELETE") {
			std::map<std::string, callback_func>::iterator i = delete_map.find(req.url());
			
			if(i!=delete_map.end()) {
					res.setLocation(req.url());
					res.setHeader("Content-Type", "text/html");
					i->second(req, res);
			} else {
				Log::inst().error("No DELETE method for: %s -> sending 404", req.url().c_str());
				res.send(404);
			}
		} else {
			res.send(501, "Not Implemented");
		}
}

void Application::get(const std::string& what, callback_func cback) 	{ get_map[what] = cback; } 
void Application::post(const std::string& what, callback_func cback) 	{ post_map[what] = cback; }
void Application::put(const std::string& what, callback_func cback) 	{ put_map[what] = cback; }
void Application::remove(const std::string& what, callback_func cback) { delete_map[what] = cback; }

void Application::listen(short port)
{
    SelectSet sel;

    std::vector<Socket*> l = Socket::tcpListenersAllInterfaces(port);
    while (l.empty())
    {
        Log::inst().error("Could not bind to any interface. Sleeping 10 seconds");
        sleep(10);
        l = Socket::tcpListenersAllInterfaces(port);
    }

    for (std::vector<Socket*>::iterator i = l.begin(); i!=l.end(); i++)
    {
        (*i)->setBlocking(false);
        sel.insertSocket(*i, SelectSet::read);
    }

    std::set<Connection*> conns;

    Log::inst().notice("Server running on port %d", port);

    while (select(sel, 10000000), (sel.count() && !quit_requested))
    {
        for (std::vector<Socket*>::iterator i = l.begin(); i!=l.end(); i++)
        {
            Socket * ls = *i;
            try
            {
                if (sel.canRead(ls))
                {
                    Socket * s = ls->accept();
                    s->setBlocking(false);
                    sel.insertSocket(s, SelectSet::read | SelectSet::write);
                    conns.insert(new Connection(s, this));
                }
            } 
            catch (const SocketException & ex)
            {
                if (!ex.isEAGAIN())
                    throw ex;
            } 
        }

        std::set<Connection*> remlist;
        for (std::set<Connection*>::iterator i = conns.begin(); i!=conns.end(); i++)
        {
            Connection * c = *i;
            Socket * s = c->getSocket();
            bool err = false;
            try {
                if (sel.canRead(s))
                    c->readStuff();
                if (sel.canWrite(s))
                    s->writeFromQueue();
                int f;
                sel.insertSocket(s, f = (SelectSet::read | 
                    ((s->writeQueueEmpty()) ? 0 : SelectSet::write))
                );
            }
            catch (const SocketException & ex)
            {
                if (!ex.isEAGAIN())
                {
                    Log::inst().error(ex.what());
                    err = true;
                }
            }
            catch (const std::exception & ex)
            {
                Log::inst().error(ex.what());
                err = true;
            }
            if (err)
            {
                sel.removeSocket(s);
                remlist.insert(c);
                delete c;
            }
        }

        for (std::set<Connection*>::iterator i = remlist.begin(); i!=remlist.end(); i++)
            conns.erase(*i);

        if (quit_requested)
            break;
    }

    for (std::set<Connection*>::iterator i = conns.begin(); i!=conns.end(); i++)
        delete *i;

    for (std::vector<Socket*>::iterator i = l.begin(); i!=l.end(); i++)
        delete *i;
}


void Application::close()
{
//    __sync_fetch_and_add(&quit_requested, 1); so not necessary.
//    assignment is atomic for ints. 
//    and even if it wasn't, it wouldn't matter since
//    1) we are not doing this from another thread, we are doing it from
//    an interrupt handler. select() being a syscall will only return
//    after and only after the handler finishes and will set errno to EINTR
    quit_requested = 1;
}
