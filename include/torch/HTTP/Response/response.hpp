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

        struct sHTTP_PARAMS {
            short code;
            std::string server_name;
            std::string location;
        };

        class response
        {
        private:
        	socket * sock;
        public:
            response(socket*);

            sHTTP_PARAMS header;
            
            // send overloads
            void send(short code = HTTP_OK, std::string what = "");
            void send(std::string what = "");

            std::string getServerName()          {return this->header.server_name;}
            void setServerName(std::string name) {this->header.server_name = name;}

            std::string getLocation()          {return this->header.location;}
            void setLocation(std::string name) {this->header.location = name;}

            void redirect(std::string to, short code = HTTP_TEMPORARY_REDIRECT);
            //void render(std::string);
    	};
	}
}

#endif