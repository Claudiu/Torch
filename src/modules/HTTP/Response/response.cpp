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

#include <fstream>
#include <sys/stat.h>
#include <magic.h>

using namespace Torch;
using namespace Torch::HTTP;
using namespace Torch::Sockets;

Response::Response(Socket * s) : sock(s) {
	// Defaults
	setServerName("Torch");
	headersSent = false;
}

void Response::redirect(const std::string& to, short code) {
	setLocation(to);
	send(code);
}
std::string Response::getMimeType(const char * staticFile) {
	std::string rez = "text/plain";

	magic_t magic;
	magic = magic_open(MAGIC_MIME_TYPE);
	magic_load(magic, NULL);
	rez = std::string(magic_file(magic, staticFile));
	magic_close(magic);

	if(rez != "text/plain") {
		if(rez == "application/x-executable")
			rez = "application/octet-stream";
		if(rez == "text/html")
			rez += ";charset=utf-8"; //TODO: Add more charsets see IANA.org
		return rez;
	} else {
		//plan B :(
		std::string sf_string = staticFile;
		std::string extension = sf_string.substr(sf_string.find_last_of(".") + 1);

		// add more here.... Dirty hack :D
		if(extension == "css")
			return "text/css";
		else if(extension == "js")
			return "application/javascript";
		else
			return rez;
	}
}

long int Response::sendFile(const std::string filepath) {
	std::ifstream file;
	file.open(filepath.c_str(), std::ifstream::binary | std::ios::in); 

	if(file.is_open() == true) 
	{
			struct stat stat_buf;
			int rc = stat(filepath.c_str(), &stat_buf);
			
			long int a = stat_buf.st_size;
			std::ostringstream tempLen;
			tempLen << stat_buf.st_size;

			setHeader("Content-Length", tempLen.str().c_str());
			setHeader("Content-Type", getMimeType(filepath.c_str()));

			long int chunkSize = 1024; // <1MB
			
			do {
				char *buffer =  new char[chunkSize];
				file.read(buffer, chunkSize);
				// buffer - std::string(buffer) adica... copiaza
				// pana intalneste /0..
				send(buffer);
				delete[] buffer;
			} while(!file.eof());

			file.close();

			return stat_buf.st_size;
	} else {
		return -1;
	}
}

void Response::send(const std::string& what) {
	send(HTTP_OK, what);
}

void Response::setCookie(const std::string& what, const std::string& to) {
	setHeader("Set-Cookie", std::string(what + "=" + to));
}

void Response::setHeader(const std::string& what, const std::string& to) {
  	if(headersSent == true)
			Log::inst().error("Headers already sent.");

    header.items[what] = to;
}

void Response::send(short code, const std::string& what) {
		std::stringstream temp;

		if(headersSent == false) {
			static std::map<short, std::string> codes;

			if (!codes.size())
			{
					codes[200] = "OK";
					codes[307] = "Temporary Redirect";
					codes[404] = "Not Found";
					codes[418] = "I'm a teapot";
					codes[501] = "Not Implemented";
			}

		temp << "HTTP/1.1 "<<code<<" "<<codes[code]<<"\n";

		if(header.items.find("Content-Length") == header.items.end() &&
					header.items.find("Transfer-Encoding") == header.items.end()) {
				temp << "Content-Length: " << what.length() << "\n";
		}

		for (std::map<std::string, std::string>::iterator it = header.items.begin(); it != header.items.end(); ++it)
			temp << it->first << ": " << it->second << "\n";

		if(header.items.find("Connection") == header.items.end()) {
			// RFC7231 defaults to keep-alive not close
			temp << "Connection: keep-alive\n";
		}

		if(header.items.find("Cache-Control") == header.items.end()) {
			temp << "Cache-Control: no-cache\n";
		}

		temp << "\n";
		headersSent = true;
	}

	temp << what;

	std::string t = temp.str();

	// I think t.size shows the position of the first null byte it meets
	uint8_t * rez = new uint8_t[t.size() + 1];
	memcpy(rez, t.c_str(), t.size());

	Log::inst().notice("Sending HTTP Response back %d bytes.", t.size());

	std::cout << what.capacity() << std::endl;
	sock->queueForWriting(rez, t.size());
}
