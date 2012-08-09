
#include <cstring>
#include <cassert>
#include <cstdarg>
#include <sstream>
#include <torch/log.hpp>

using namespace Torch;

log::log() : use_stdout(true), append(false)
{
	memset(logs, 0, sizeof(logs));

	log_names[LOG_ERROR] = "./logs/error.log";
	log_names[LOG_WARN] = "./logs/warn.log";
	log_names[LOG_NOTICE] = "./logs/notice.log";
	log_names[LOG_ACCESS] = "./logs/access.log";
	log_names[LOG_CLIENT] = "./logs/client.log";
}

log::~log()
{
	close_logs();
}

void log::error(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_ERROR, fmt, ap);
	va_end(ap);
}

void log::warn(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_WARN, fmt, ap);
	va_end(ap);
}

void log::notice(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_NOTICE, fmt, ap);
	va_end(ap);
}

void log::access(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_ACCESS, fmt, ap);
	va_end(ap);
}

void log::client(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	print(LOG_CLIENT, fmt, ap);
	va_end(ap);
}

void log::print(log_level lvl, const char* fmt, va_list ap)
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
		std::string timestamp = get_timestamp();
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

void log::open_logs()
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
			throw string_exception("Cannot open log \"") << 
				  log_names[i] << "\"";
		}
	}
}

void log::close_logs()
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

void log::set_log_file(log_level lvl, const std::string& file)
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
			throw string_exception("Cannot open log \"") << file << "\"";
		}
	}

	log_names[lvl] = file;
}

void log::set_use_stdout(bool b)
{
	use_stdout = b;
}

void log::set_append(bool b)
{
	append = b;
}

std::string log::get_timestamp()
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
