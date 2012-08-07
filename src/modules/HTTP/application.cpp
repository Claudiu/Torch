#include <torch/HTTP/application.hpp>
#include <torch/sockets.hpp>
#include <torch/logs.hpp>
#include <set>

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
                    uint8_t b[512];
                    ssize_t s = sock->read(b, 511);
                    if (!s)
                        throw string_exception("Connection closed");
                    b[s] = '\0';
                    Torch::toLog(std::string((char*)b));
                }
                
                ~connection() {
                    delete sock;
                }
            };
    }
}


void application::listen(short port)
{
    std::vector<socket*> l = socket::tcp_listeners_all_interfaces(port);
    for (std::vector<socket*>::iterator i = l.begin(); i!=l.end(); i++)
        (*i)->set_blocking(false);

    std::set<connection*> conns;

    while (1) //this should be a select;
    {
        for (std::vector<socket*>::iterator i = l.begin(); i!=l.end(); i++)
        {
            socket * ls = *i;
            try
            {
                if (ls->can_accept())
                {
                    socket * s = ls->accept();
                    s->set_blocking(false);
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
                if (s->can_read())
                    c->read_stuff();
                if (s->can_write())
                    s->write_from_queue();
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
