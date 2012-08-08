#include <torch/http.hpp>
#include <torch/sockets.hpp>
#include <torch/logs.hpp>
#include <set>
#include <string.h>
#include <sstream>
#include <stdio.h>

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


                    response res(sock);
                    res.send("<h1>Torch is fucking awesome</h1><p>This is the first html test in history</p>");

                 //this frees b
                }
                
                ~connection() {
                    delete sock;
                }
            };
        }
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

    while (select(sel, 10000000), sel.count()) //this should be a select;
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
                    Torch::toLog(ex.what());
                    err = true;
                }
            }
            catch (const std::exception & ex)
            {
                Torch::toLog(ex.what());
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
