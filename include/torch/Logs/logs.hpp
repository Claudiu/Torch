#include <time.h>
#include <iostream>

#define LOG_DATE_FORMAT "[%d-%m-%Y %H:%M:%S] "

#ifndef _LOGS_H
#define _LOGS_H

namespace Torch {
    class string_exception : public std::exception
    {
        private:
        const char * s;
        public:
        string_exception(const char * str) : s(str) {};
        string_exception(const string_exception & o) : s(o.s) {};
        const char * what() const throw() { return s; }
    };

	std::string getTimestamp(
		const char* format = LOG_DATE_FORMAT
		);

	void toLog(
		std::string msg, 
		unsigned short Level = 0 
		);
}

#endif

