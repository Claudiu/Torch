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

#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

#include <torch/HTTP/headers.hpp>
#include <set>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <map>

namespace Torch {
    namespace Sockets {
        class Socket;
    }

	namespace HTTP {

        struct sHTTP_PARAMS {
            short code;
            std::string server_name;
            std::string location;

            std::map <std::string, std::string> items;
            //HeaderMap items;
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

            void setServerName(std::string name) {
                setHeader("Server", name);
            }

            bool HeaderExists(std::string what) {
                std::map<std::string, std::string>::iterator it;
                it = header.items.find(what);

                if(it != header.items.end())
                    return true;
                else
                    return false;
            }

            void setHeader(std::string what, std::string to);

            std::string getLocation() {
                if(HeaderExists("Location"))
                    return header.location;
                else {
                    setHeader("Location", "/"); //TODO
                    return getLocation();
                }
            }

            void setLocation(std::string what) {
                setHeader("Location", what);
            }

            void redirect(std::string to, short code = HTTP_TEMPORARY_REDIRECT);
            //void render(std::string);
    	};
	}
}

#endif
