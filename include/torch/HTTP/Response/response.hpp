#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

#include <torch/HTTP/headers.hpp>
#include <set>
#include <string.h>
#include <sstream>
#include <stdio.h>

namespace Torch {
    namespace Sockets {
        class Socket;
    }

	namespace HTTP {

        struct sHTTP_PARAMS {
            short code;
            std::string server_name;
            std::string location;
        };

        class Response
        {
        private:
        	Sockets::Socket * sock;
        public:
            Response(Sockets::Socket *);

            sHTTP_PARAMS header;
            
            // send overloads
            void send(short code = HTTP_OK, std::string what = "");
            void send(std::string what = "");

            std::string getServerName()          {return header.server_name;}
            void setServerName(std::string name) {header.server_name = name;}

            std::string getLocation()          {return header.location;}
            void setLocation(std::string name) {header.location = name;}

            void redirect(std::string to, short code = HTTP_TEMPORARY_REDIRECT);
            //void render(std::string);
    	};
	}
}

#endif
