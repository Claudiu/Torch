#include "logs.hpp"

std::string getTimestamp(const char* format = LOG_DATE_FORMAT) {
	char date[30];
	time_t t = time(0);
	struct tm *tm;

	tm = localtime(&t);
	strftime(date, sizeof(date), format, tm);
	
	return std::string(date);
}

void toLog(std::string msg, unsigned short Level = 0 ) {
	if(Level == 0) {
		std::cout << getTimestamp() << msg << std::endl;
	} else {
		std::cerr << getTimestamp() << msg << std::endl;
	}
}