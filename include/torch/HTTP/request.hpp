#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <vector>
#include <string>

namespace Torch {
    namespace HTTP {
        class request 
        {
            protected:
            std::string _method, _url, _version;
            std::vector<std::string> _headers;

            public:
            const std::string & method() { return _method; }
            const std::string & url() { return _url; }
            const std::string & http_version() { return _version; }
            const std::vector<std::string> headers() { return _headers; }

            request(const std::string & data);
        };
    }
}

#endif
