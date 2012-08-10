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

#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <vector>
#include <string>

namespace Torch {
    namespace HTTP {
        class Request 
        {
            protected:
            std::string _method, _url, _version;
            std::vector<std::string> _headers;

            public:
            const std::string & method() const { return _method; }
            const std::string & url() const { return _url; }
            const std::string & httpVersion() const { return _version; }
            const std::vector<std::string> headers() const { return _headers; }

            Request(const std::string & data);
        };
    }
}

#endif
