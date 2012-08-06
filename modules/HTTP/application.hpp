#ifndef __HTTP_APPLICATION_H__
#define __HTTP_APPLICATION_H__

namespace Torch {
	namespace HTTP {
		struct application
		{
			void get(std::string what) {
				std::cout << "GET /" << what << "\r\n";
			}
			void put();
			void post();

			void listen(short port = DEFAULT_PORT) {

			}
		};
	}
}

#endif