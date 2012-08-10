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


#include <cstring>
#include <cassert>
#include <cstdarg>
#include <sstream>
#include <torch/log.hpp>

using namespace Torch;

Log::Log() : use_stdout(true), append(false)
{
	memset(logs, 0, sizeof(logs));

	log_names[LOG_ERROR] = "./logs/error.log";
	log_names[LOG_WARN] = "./logs/warn.log";
	log_names[LOG_NOTICE] = "./logs/notice.log";
	log_names[LOG_ACCESS] = "./logs/access.log";
	log_names[LOG_CLIENT] = "./logs/client.log";
}

Log::~Log()
{
	closeLogs();
}

void Log::error(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_ERROR, fmt, ap);
	va_end(ap);
}

void Log::warn(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_WARN, fmt, ap);
	va_end(ap);
}

void Log::notice(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_NOTICE, fmt, ap);
	va_end(ap);
}

void Log::access(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_ACCESS, fmt, ap);
	va_end(ap);
}

void Log::client(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_CLIENT, fmt, ap);
	va_end(ap);
}

void Log::print(log_level lvl, const char* fmt, va_list ap)
{	
	char *buf, *tmp;
	int size = 100, r;

	assert(lvl < LOG_NUM);
	
	buf = new char[size + 1];
	while (true) {
		r = vsnprintf(buf, size, fmt, ap);

		if (-1 < r && r < size) {
			break;
		}

		size = (r > -1) ? (r + 1) : (size <<= 1);
		tmp = buf;

		if (!(buf = new (std::nothrow) char[size + 1])) {
			delete[] tmp;
			return;
		}

		delete[] tmp;
	}	

	if (buf)
	{
		std::string timestamp = getTimestamp();
		const char* level_name[] = {
			"\033[31mERROR",
			"\033[33mWARNING",
			"\033[34mNOTICE",
			"\033[37mCLIENT",
			"\033[32mACCESS"
		};

		if (use_stdout)
		{
			printf("%s [%s] %s\033[0m\n", level_name[lvl], timestamp.c_str(), buf);
		}

		if (logs[lvl] != NULL)
		{
			fprintf(logs[lvl], "[%s] %s\n", timestamp.c_str(), buf);
		}
	}
}

void Log::openLogs()
{
	for (int i = 0; i < LOG_NUM; ++i)
	{
		if (logs[i] != NULL)
		{
			fclose(logs[i]);
		}

		logs[i] = fopen(log_names[i].c_str(), append ? "a" : "w");
		if (!logs[i]) 
		{
			throw StringException("Cannot open log \"") << 
				  log_names[i] << "\"";
		}
	}
}

void Log::closeLogs()
{
	for (size_t i = 0; i < LOG_NUM; ++i)
	{
		if (logs[i] != NULL)
		{
			fclose(logs[i]);
			logs[i] = NULL;
		}
	}
}

void Log::setLogFile(log_level lvl, const std::string& file)
{
	assert(lvl < LOG_NUM);
	if (log_names[lvl] != file)
	{
		if (logs[lvl] != NULL)
		{
			fclose(logs[lvl]);
		}

		logs[lvl] = fopen(file.c_str(), append ? "a" : "w");
		if (!logs[lvl])
		{
			throw StringException("Cannot open log \"") << file << "\"";
		}
	}

	log_names[lvl] = file;
}

void Log::setUseStdout(bool b)
{
	use_stdout = b;
}

void Log::setAppend(bool b)
{
	append = b;
}

std::string Log::getTimestamp()
{
	time_t rawtime;
	struct tm * tm;

	time(&rawtime);
	tm = gmtime(&rawtime);

	std::stringstream ss;
	ss << tm->tm_mday << '/' << tm->tm_mon + 1 << '/' << tm->tm_year + 1900 << ':'
	   << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec;

	return ss.str();
}
