#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

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


namespace Torch {
	namespace HTTP {
        class response
        {
        private:
        	socket * sock;
        public:
            response(socket*);
            void send(std::string, short code = 200);
    	};
	}
}

#endif