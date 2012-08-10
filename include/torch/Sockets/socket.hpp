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
        class socket_exception : public std::exception
        {
            int err;
            public:
            socket_exception(int e) : err(e) {};
            socket_exception(const socket_exception & o) : err(o.err) {}
            bool is_eagain() const;
            const char * what() const throw();
        };
    
        struct select_struct;

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
            bool write_queue_empty() { return wq.empty(); }

            static std::vector<socket*> tcp_listeners_all_interfaces(short port);

            int file_descriptor() { return fd; }
        };
    }
}

#endif
