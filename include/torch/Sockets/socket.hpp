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
