
#include <sstream>
#include <torch/util.hpp>

using namespace Torch;

string_exception& string_exception::operator << (int i)
{
	std::stringstream ss;
	ss << s << i;
	s = ss.str();
	return *this;
}

string_exception& string_exception::operator << (const std::string& str)
{
	s += str;
	return *this;
}
