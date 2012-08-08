#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

#include <torch/HTTP/application.hpp>
#include <torch/HTTP/Response/response.hpp>
#include <torch/sockets.hpp>
#include <torch/logs.hpp>
#include <set>
#include <string.h>
#include <sstream>
#include <stdio.h>

using namespace Torch;
using namespace Torch::HTTP;
using namespace Torch::Sockets;

namespace Torch {
	namespace HTTP {
        class response
        {
        public:
            response(socket * sock) {
                this->sock = sock;
            }

            socket * sock;
            void send(std::string what) {
                std::stringstream temp;

                temp << "HTTP/1.1 200 OK\n";
                temp << "Content-Type: text/html\n";
                temp << "Content-Length: " << what.length() << "\n";
                temp << "Connection: close\n";
                temp << "\n";
                temp << what;

                std::string t = temp.str();
                const char * kitty = t.c_str();

                size_t kitty_tail_length = strlen(kitty); 
                uint8_t * rez = new uint8_t[kitty_tail_length];

                memcpy(rez, kitty, kitty_tail_length);

                Torch::toLog(std::string((char*)rez));

                sock->queue_for_writing(rez, kitty_tail_length);
            }
    	};
	}
}

#endif