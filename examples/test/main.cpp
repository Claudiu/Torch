#include <iostream>
#include <torch/http.hpp>
#include <torch/logs.hpp>

namespace Routes {
	void index() {
		Torch::toLog("Found Index");
	}
};

int main(int argc, char const *argv[])
{
	try {
		Torch::HTTP::application app;
		
		app.get("/", Routes::index);
		app.get("/account/:name", Routes::index);

	} catch (std::exception e) {
		Torch::toLog(e.what());
	}
	return 0;
}
