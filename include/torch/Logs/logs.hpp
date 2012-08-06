#include <time.h>
#include <iostream>

#define LOG_DATE_FORMAT "[%d-%m-%Y %H:%M:%S] "

#ifndef _LOGS_H
#define _LOGS_H

namespace Torch {
	std::string getTimestamp(
		const char* format = LOG_DATE_FORMAT
		);

	void toLog(
		std::string msg, 
		unsigned short Level = 0 
		);
}

#endif

