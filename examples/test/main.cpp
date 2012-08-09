#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <torch/http.hpp>
#include <torch/log.hpp>

namespace Routes {
	void index(const Torch::HTTP::request & req, Torch::HTTP::response & res) {
        res.send(std::string("<h1>Torch is fucking awesome</h1><p>This is the first html test in history</p> <p>") + req.url() + "</p>");
        log::inst().client("Found index");
	}
};

// This is one of the worst hacks ever written by me
Torch::HTTP::application app;
void sigint(int)
{
	app.close();
	printf ("The torch was extinguished :(\n\n");
	exit(0);
}

int main(int argc, char const *argv[])
{
	try {
		log::inst().open_logs();

		signal(SIGINT, sigint);

		app.get("/", &Routes::index);
		app.get("/account/:name", &Routes::index);

        app.listen(8080);

        log::inst().close_logs();

	} catch (std::exception & e) {
		log::inst().error(e.what());
	}

	return 0;
}
