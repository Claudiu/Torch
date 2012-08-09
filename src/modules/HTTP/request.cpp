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

    std::string msg;
    msg += "method: \"" + _method + "\"";
    msg += "uri: \"" + _url + "\"";
    msg += "version: \"" + _version + "\"";

    for (int i = 0; i<_headers.size(); i++)
        msg += "header: \"" + _headers[i] + "\"";

    log::inst().access(msg.c_str());
}
