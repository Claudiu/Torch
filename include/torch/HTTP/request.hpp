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
