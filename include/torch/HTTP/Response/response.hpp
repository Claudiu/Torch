#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

#include <torch/http.hpp>
#include <torch/sockets.hpp>
#include <torch/log.hpp>
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
            void redirect(std::string to, short code = HTTP_TEMPORARY_REDIRECT);
            //void render(std::string);
    	};
	}
}

#endif