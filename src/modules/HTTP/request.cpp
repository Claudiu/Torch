#include <torch/http.hpp>
#include <sstream>

using namespace Torch;
using namespace Torch::HTTP;

request::request(const std::string & data)
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
    toLog(std::string("method: \"") + _method + "\"");
    toLog(std::string("uri: \"") + _url + "\"");
    toLog(std::string("version: \"") + _version + "\"");
    for (int i = 0; i<_headers.size(); i++)
        toLog(std::string("header: \"") + _headers[i] + "\"");
}
