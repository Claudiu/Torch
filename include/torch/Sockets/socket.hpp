#ifndef __SOCKETS_SOCKET_H__
#define __SOCKETS_SOCKET_H__

#include <stdint.h>
#include <list>
#include <vector>

namespace Torch {
    namespace Sockets {
        class socket
        {
            protected:
            int fd;
            size_t woff;
            std::list< std::pair<void *, size_t> > wq;

            public:
            socket(int fd);
            ~socket();

            void set_blocking(bool blocking);
            bool blocking();

            bool error();
            size_t can_read(); //bytes to read
            bool can_write();
            bool can_accept();

            ssize_t read(void * buf, size_t size);
            ssize_t write(void * buf, size_t size);
            socket * accept();

            void queue_for_writing(void * buf, size_t size); //feed me new-allocated pointers and I'll handle deleting them
            void write_from_queue();

            static std::vector<socket*> tcp_listeners_all_interfaces(short port);
        };
    }
}

#endif
