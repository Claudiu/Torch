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
#include <torch/sockets.hpp>
#include <torch/util.hpp>
#include <torch/log.hpp>

using namespace Torch;
using namespace Torch::HTTP;
using namespace Torch::Sockets;

namespace Torch{
    namespace HTTP {
        class connection //this should parse http requests and send them to a function of application to dispatch them 
            {
                private:
                socket * sock;
                application * app;

                public:
                socket * get_socket() { return sock; }
                
                connection(socket * s, application * a) : sock(s), app(a) {
                }
                
                void read_stuff()
                {
                    uint8_t * b = new uint8_t[512];
                    ssize_t s = sock->read(b, 511);
                    if (!s)
                    {
                        delete[] b;
                        throw string_exception("Connection closed");
                    }
                    b[s] = '\0';
                    request req(std::string((char*)b));
                    delete[] b;

                    response res(sock);
                    app->dispatch_request(req, res);
                }
                
                ~connection() {
                    delete sock;
                }
            };
        }
}

void application::dispatch_request(const request & req, response & res)
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

void application::get(std::string what, callback_func cback)
{
    get_map[what] = cback;
}

void application::listen(short port)
{
    select_struct sel;

    std::vector<socket*> l = socket::tcp_listeners_all_interfaces(port);
    if (l.empty())
        throw string_exception("Could not bind to any interface");
    for (std::vector<socket*>::iterator i = l.begin(); i!=l.end(); i++)
    {
        (*i)->set_blocking(false);
        sel.insert_socket(*i, select_struct::read);
    }

    std::set<connection*> conns;

    while (select(sel, 10000000), sel.count() && quit_requested == 0) //this should be a select;
    {
        for (std::vector<socket*>::iterator i = l.begin(); i!=l.end(); i++)
        {
            socket * ls = *i;
            try
            {
                if (sel.can_read(ls))
                {
                    socket * s = ls->accept();
                    s->set_blocking(false);
                    sel.insert_socket(s, select_struct::read | select_struct::write);
                    conns.insert(new connection(s, this));
                }
            } 
            catch (const socket_exception & ex)
            {
                if (!ex.is_eagain())
                    throw ex;
            } 
        }

        std::set<connection*> remlist;
        for (std::set<connection*>::iterator i = conns.begin(); i!=conns.end(); i++)
        {
            connection * c = *i;
            socket * s = c->get_socket();
            bool err = false;
            try {
                if (sel.can_read(s))
                    c->read_stuff();
                if (sel.can_write(s))
                    s->write_from_queue();
                sel.insert_socket(s, select_struct::read | (( s->write_queue_empty())?0:select_struct::write));
            }
            catch (const socket_exception & ex)
            {
                if (!ex.is_eagain())
                {
                    log::inst().error(ex.what());
                    err = true;
                }
            }
            catch (const std::exception & ex)
            {
                log::inst().error(ex.what());
                err = true;
            }
            if (err)
            {
                sel.remove_socket(c->get_socket());
                remlist.insert(c);
                delete c;
            }
        }

        for (std::set<connection*>::iterator i = remlist.begin(); i!=remlist.end(); i++)
            conns.erase(*i);
    }

    for (std::vector<socket*>::iterator i = l.begin(); i!=l.end(); i++)
        delete *i;
}


void application::close()
{
    __sync_fetch_and_add(&quit_requested, 1);
}
