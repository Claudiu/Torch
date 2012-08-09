
#ifndef __LOGS_HPP__
#define __LOGS_HPP__

#include <cstdio>
#include <string>
#include <torch/util.hpp>

namespace Torch {
	class log;

	enum log_level
	{
		LOG_ERROR,
		LOG_WARN,
		LOG_NOTICE,
		LOG_CLIENT,
		LOG_ACCESS,
		LOG_NUM
	};

    class log : public singleton<log>
    {
    	friend class singleton<log>;
    	friend class log_stream;

    	std::string log_names[LOG_NUM];
    	FILE * logs[LOG_NUM];

    	bool use_stdout;
    	bool append;

    	public:
    	virtual ~log();

    	void set_log_file(log_level lvl, const std::string& str);
    	void set_use_stdout(bool b);
    	void set_append(bool b);

    	void open_logs();
    	void close_logs();

    	void error(const char* fmt, ...);
    	void client(const char* fmt, ...);
    	void warn(const char* fmt, ...);
    	void notice(const char* fmt, ...);
    	void access(const char* fmt, ...);

   		private:
    	log();

    	void print(log_level lvl, const char* fmt, va_list list);
    	std::string get_timestamp();
    };

}

#endif

