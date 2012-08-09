#ifndef __SOCKETS_SOCKET_H__
#define __SOCKETS_SOCKET_H__

#include <stdint.h>
#include <sys/types.h>
#include <list>
#include <vector>
#include <iostream>

namespace Torch {
    namespace Sockets {
        class SocketException : public std::exception
        {
            int err;
            public:
            SocketException(int e) : err(e) {};
            SocketException(const SocketException & o) : err(o.err) {}
            bool isEAGAIN() const;
            const char * what() const throw();
        };
    
        class Socket
        {
            protected:
            int fd;
            size_t woff;
            std::list< std::pair<uint8_t *, size_t> > wq;

            public:
            Socket(int fd);
            ~Socket();

            void setBlocking(bool blocking);
            bool blocking();

            bool error();
            bool canRead();
            bool canWrite();
            bool canAccept();

            ssize_t readPeek(uint8_t * buf, size_t size);

            ssize_t read(uint8_t * buf, size_t size);
            ssize_t write(uint8_t * buf, size_t size);
            Socket * accept();

            void queueForWriting(uint8_t * buf, size_t size); //feed me new-allocated pointers and I'll handle deleting them
            void writeFromQueue();
            bool writeQueueEmpty() { return wq.empty(); }

            static std::vector<Socket*> tcpListenersAllInterfaces(short port);

            int fileDescriptor() { return fd; }
        };
    }
}

#endif
