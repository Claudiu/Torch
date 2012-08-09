
#ifndef __LOGS_HPP__
#define __LOGS_HPP__

#include <cstdio>
#include <string>
#include <torch/util.hpp>

namespace Torch {
	class Log;

	enum log_level
	{
		LOG_ERROR,
		LOG_WARN,
		LOG_NOTICE,
		LOG_CLIENT,
		LOG_ACCESS,
		LOG_NUM
	};

    class Log : public Singleton<Log>
    {
    	friend class Singleton<Log>;
    	friend class log_stream;

    	std::string log_names[LOG_NUM];
    	FILE * logs[LOG_NUM];

    	bool use_stdout;
    	bool append;

    	public:
    	virtual ~Log();

    	void setLogFile(log_level lvl, const std::string& str);
    	void setUseStdout(bool b);
    	void setAppend(bool b);

    	void openLogs();
    	void closeLogs();

    	void error(const char* fmt, ...);
    	void client(const char* fmt, ...);
    	void warn(const char* fmt, ...);
    	void notice(const char* fmt, ...);
    	void access(const char* fmt, ...);

   		private:
    	Log();

    	void print(log_level lvl, const char* fmt, va_list list);
    	std::string getTimestamp();
    };

}

#endif

