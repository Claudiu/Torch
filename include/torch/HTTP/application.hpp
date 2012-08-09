#ifndef __HTTP_APPLICATION_H__
#define __HTTP_APPLICATION_H__

#include <string>
#include <map>

namespace Torch {
	namespace HTTP {
        
        class Request;
        class Response;
        class Connection;

		class Application
		{
            public:
            typedef void (*callback_func)(const Request &, Response &);
            volatile int quit_requested;

            protected:
            std::map<std::string, callback_func> get_map;

            void dispatchRequest(const Request &, Response &);
            friend class Connection;

            public:
            Application() : quit_requested(0) { }
			void get(std::string what, callback_func cback);
			void put();
			void post();

            static const short DEFAULT_PORT = 80;

			void listen(short port = DEFAULT_PORT);
            void close();
		};
	}
}

#endif
