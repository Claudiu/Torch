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
#include <string.h>
#include <sstream>
#include <stdio.h>

#include <torch/http.hpp>
#include <torch/Sockets.hpp>
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

void Application::dispatchRequest(const Request & req, Response & res)
{
    if (req.method() == "GET")
    {
        std::map<std::string, callback_func>::iterator i = get_map.find(req.url());
        if (i!=get_map.end())
            i->second(req, res);
        else
        {
            //res.send(std::string("<!DOCTYPE HTML> <html lang=\"en-US\"> <head> <meta charset=\"UTF-8\"> <title>Torch: Page Not Found</title> <style type=\"text/css\"> body {font-family: Arial, Helvetica, sans-serif; font-size: 13px; background-color: #EBD9ED; } h1 {color: #641A67; font-family: Georgia; } .wrap {width: 500px; margin: 150px auto; border: 5px solid #e5cde8; padding: 50px; background-color: #f1e5f2; border-radius: 10px; } </style> </head> <body> <div class=\"wrap\"> <h1>Budi was here..now this page stinks.</h1> <p>Oh noes!</p> </div> </body> </html>"));
        }
    }
}

void Application::get(std::string what, callback_func cback)
{
    get_map[what] = cback;
}

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
