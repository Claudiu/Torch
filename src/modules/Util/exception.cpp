
#include <sstream>
#include <torch/util.hpp>

using namespace Torch;

StringException & StringException::operator << (int i)
{
	std::stringstream ss;
	ss << s << i;
	s = ss.str();
	return *this;
}

StringException & StringException::operator << (const std::string& str)
{
	s += str;
	return *this;
}
