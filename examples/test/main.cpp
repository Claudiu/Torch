#include <iostream>
#include <torch/http.hpp>
#include <torch/logs.hpp>

namespace Routes {
	void index(const Torch::HTTP::request & req, Torch::HTTP::response & res) {
        res.send(std::string("<h1>Torch is fucking awesome</h1><p>This is the first html test in history</p> <p>") + req.url() + "</p>");
		Torch::toLog("Found Index");
	}
};

int main(int argc, char const *argv[])
{
	try {
		Torch::HTTP::application app;
		
		app.get("/", &Routes::index);
		app.get("/account/:name", &Routes::index);

        app.listen(3000);

	} catch (std::exception e) {
		Torch::toLog(e.what());
	}
	return 0;
}
