//
// Copyright (C) 2012 Claudiu Tasca <clawgm [at] gmail.com>, Marius Petcu <marius
// [at] porkholt.org>, Nandor Licker <licker.nandor [at] gmail.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


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

