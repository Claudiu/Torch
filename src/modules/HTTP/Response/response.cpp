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

#include <torch/HTTP/Response/response.hpp>
#include <torch/sockets.hpp>
#include <torch/log.hpp>

using namespace Torch;
using namespace Torch::HTTP;
using namespace Torch::Sockets;

Response::Response(Socket * s) : sock(s) {
	setServerName("Torch");
	setLocation("");
}

void Response::redirect(std::string to, short code) {
	setLocation(to);
	send(code);
}


void Response::send(std::string what) {
	send(HTTP_OK, what);
}

void Response::send(short code, std::string what) {
	std::stringstream temp;

    static std::map<short, std::string> * codes = NULL;
    if (!codes)
    {
        codes = new std::map<short, std::string>;
        (*codes)[200] = "OK";
        (*codes)[307] = "Temporary Redirect";
        (*codes)[404] = "Not Found";
        (*codes)[418] = "I'm a teapot";

    }

	temp << "HTTP/1.1 "<<code<<" "<<(*codes)[code]<<"\n";
	
	if(what.length() != 0)
		temp << "Content-Type: text/html\n";

	if(!getLocation().empty())
		temp << "Location: " << getLocation() << "\n";

	if(!getServerName().empty())
		temp << "Server: " << getServerName() << "\n";
	
	temp << "Content-Length: " << what.length() << "\n";
	temp << "Connection: close\n";
	temp << "\n";
	temp << what;

	std::string t = temp.str();
	const char * kitty = t.c_str();

	size_t kitty_tail_length = strlen(kitty); 
	uint8_t * rez = new uint8_t[kitty_tail_length];

	memcpy(rez, kitty, kitty_tail_length);

	Log::inst().notice("Sending HTTP Response back.");

	sock->queueForWriting(rez, kitty_tail_length);
}
