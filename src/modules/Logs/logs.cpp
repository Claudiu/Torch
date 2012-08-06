#include <torch/Logs/logs.hpp>

std::string Torch::getTimestamp(const char* format) {
	char date[30];
	time_t t = time(0);
	struct tm *tm;

	tm = localtime(&t);
	strftime(date, sizeof(date), format, tm);
	
	return std::string(date);
}

void Torch::toLog(std::string msg, unsigned short Level) {
	if(Level == 0) {
		std::cout << getTimestamp() << msg << std::endl;
	} else {
		std::cerr << getTimestamp() << msg << std::endl;
	}
}
