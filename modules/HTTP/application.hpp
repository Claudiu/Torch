#ifndef __HTTP_APPLICATION_H__
#define __HTTP_APPLICATION_H__

typedef void (*Callback_func)();

namespace Torch {
	namespace HTTP {
		struct application
		{
			void get(std::string what, Callback_func cback) {
				if(what == "/")
					cback();
			}
			
			void put();
			void post();

			void listen(short port = DEFAULT_PORT) {

			}
		};
	}
}

#endif