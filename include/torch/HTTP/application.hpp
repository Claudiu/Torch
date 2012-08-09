#ifndef __HTTP_APPLICATION_H__
#define __HTTP_APPLICATION_H__

#include <string>
#include <map>



namespace Torch {
	namespace HTTP {
        
        class request;
        class response;
        class connection;

		class application
		{
            public:
            typedef void (*callback_func)(const request &, response &);
            bool quit_requested;

            protected:
            std::map<std::string, callback_func> get_map;

            void dispatch_request(const request &, response &);
            friend class connection;

            public:
            application() : quit_requested(false) { }
			void get(std::string what, callback_func cback);
			void put();
			void post();

            static const short DEFAULT_PORT = 80;

			void listen(short port = DEFAULT_PORT);
            void close() { quit_requested = true; }
		};
	}
}

#endif
