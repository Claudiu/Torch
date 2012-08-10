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

using namespace Torch;
using namespace Torch::HTTP;

Request::Request(const std::string & data)
{
    std::stringstream s(data);
    s>>_method>>_url>>_version;
    std::string ss;
    while (s.good())
    {
        getline(s, ss);
        if (ss.size() && (ss[ss.size()-1]=='\r'))
            ss.resize(ss.size()-1);
        if (ss.empty()) continue;
        _headers.push_back(ss);
    }

    std::string msg;
    msg += "Client requested using \"" + _method + "\"";
    msg += " \"" + _url + "\"";
    //msg += "version: \"" + _version + "\"";

    //for (int i = 0; i<_headers.size(); i++)
      //  msg += "header: \"" + _headers[i] + "\"";

    Log::inst().access(msg.c_str());
}
