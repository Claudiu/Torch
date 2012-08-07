#ifndef __SOCKETS_SOCKET_H__
#define __SOCKETS_SOCKET_H__

#include <stdint.h>
#include <sys/types.h>
#include <list>
#include <vector>
#include <iostream>

namespace Torch {
    namespace Sockets {
        class socket_exception : public std::exception
        {
            int err;
            public:
            socket_exception(int e) : err(e) {};
            socket_exception(const socket_exception & o) : err(o.err) {}
            bool is_eagain() const;
            const char * what() const throw();
        };

        class socket
        {
            protected:
            int fd;
            size_t woff;
            std::list< std::pair<uint8_t *, size_t> > wq;

            public:
            socket(int fd);
            ~socket();

            void set_blocking(bool blocking);
            bool blocking();

            bool error();
            bool can_read();
            bool can_write();
            bool can_accept();

            ssize_t read_peek(uint8_t * buf, size_t size);

            ssize_t read(uint8_t * buf, size_t size);
            ssize_t write(uint8_t * buf, size_t size);
            socket * accept();

            void queue_for_writing(uint8_t * buf, size_t size); //feed me new-allocated pointers and I'll handle deleting them
            void write_from_queue();

            static std::vector<socket*> tcp_listeners_all_interfaces(short port);
        };
    }
}

#endif
