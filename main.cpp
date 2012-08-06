#include <iostream>
#include "modules/http.hpp"

namespace Routes {
	void index() {
		std::cout << "Reached Index";
	}
};

int main(int argc, char const *argv[])
{
	Torch::HTTP::application app;
	
	app.get("/", Routes::index);
	app.get("/out", Routes::index);

	return 0;
}