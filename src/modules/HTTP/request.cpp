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

#include <torch/http.hpp>
#include <torch/log.hpp>
#include <sstream>
#include <iostream>

using namespace Torch;
using namespace Torch::HTTP;

const std::string Request::getCookie(const std::string what) const {
	std::string rez = "";
	std::string cookieString = getHeader("Cookie");

	if(cookieString == "")
		return rez;
	else {
		std::size_t where = cookieString.find(what);
		// if the cookie contains ; in any way... we are so f*cked
		// TODO: Fix this

		if(where!=std::string::npos)
			rez = cookieString.substr(where, cookieString.find(";", where));
			rez = rez.substr(rez.find("=") + 1);
	}

	return rez;
}

const std::string Request::getHeader(const std::string what) const {
	std::string rez = "";
	for(std::vector<std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
		if(it->substr(0, what.size()) == what) {
			size_t pos = it->find_first_of(":");
			rez = it->substr(pos + 1);
		
			// eliminate header spaces and get to the good part
			size_t spaces = 0;
			while(rez[spaces] == ' ')
				spaces++;
			rez = rez.substr(spaces);
			
			break;
		}
	}

	return rez;
}

Request::Request(const std::string & data)
{
    std::stringstream s(data);
    s>>_method>>_url>>_version;
    
		std::string ss;
		short isHeader = 0;

    while (s.good())
    {
        getline(s, ss);
        if (ss.size() && (ss[ss.size()-1]=='\r'))
            ss.resize(ss.size()-1);
        		
				if (ss.empty()) {
					isHeader++;
					continue;
				}
        
				// this has to be the best hack ever
				// since the first line is EMPTY.. this gets autoset to false
				// in the previous version
				if (isHeader == 1)
					_headers.push_back(ss);
				else
					_body = ss;
		}



    std::string msg;
    msg += "Client requested using \"" + _method + "\"";
    msg += " \"" + _url + "\"";

    Log::inst().access("%s", msg.c_str());
}
