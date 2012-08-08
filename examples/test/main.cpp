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
		
		app.get("/", Routes::index, Routes::index);
		app.get("/account/:name", Routes::index);

        app.listen(3000);

	} catch (std::exception e) {
		Torch::toLog(e.what());
	}
	return 0;
}
