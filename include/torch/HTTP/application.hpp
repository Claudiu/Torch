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

#ifndef __HTTP_APPLICATION_H__
#define __HTTP_APPLICATION_H__

#include <string>
#include <map>



namespace Torch {
	namespace HTTP {
        
        class request;
        class response;
        class connection;

		class application
		{
            public:
            typedef void (*callback_func)(const request &, response &);
            int quit_requested;

            protected:
            std::map<std::string, callback_func> get_map;

            void dispatch_request(const request &, response &);
            friend class connection;

            public:
            application() : quit_requested(0) { }
			void get(std::string what, callback_func cback);
			void put();
			void post();

            static const short DEFAULT_PORT = 80;

			void listen(short port = DEFAULT_PORT);
            void close();
		};
	}
}

#endif
