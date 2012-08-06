#include <iostream>
#include "modules/http.hpp"
#include "modules/Logs/logs.hpp"

namespace Routes {
	void index() {
		std::cout << "Awesome";
	}
};

int main(int argc, char const *argv[])
{
	try {
		Torch::HTTP::application app;
		
		app.get("/", Routes::index);
		app.get("/account/:name", Routes::index);

	} catch (std::exception e) {
		toLog(e.what());
	}
	return 0;
}