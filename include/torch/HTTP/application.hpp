#ifndef __HTTP_APPLICATION_H__
#define __HTTP_APPLICATION_H__

#include <string>


typedef void (*Callback_func)();

namespace Torch {
	namespace HTTP {

		class application
		{
            public:

			void get(std::string what, Callback_func cback, ...) {
				if(what == "/") { 
					cback();
				}
			}
			
			void put();
			void post();

            static const short DEFAULT_PORT = 80;

			void listen(short port = DEFAULT_PORT);
		};
	}
}

#endif
