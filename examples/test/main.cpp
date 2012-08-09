#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <torch/http.hpp>
#include <torch/log.hpp>

namespace Routes {
	void index(const Torch::HTTP::request & req, Torch::HTTP::response & res) {
        res.send(std::string("<h1>Torch is fucking awesome</h1><p>This is the first html test in history</p> <p>") + req.url() + "</p>");
	}

	void account(const Torch::HTTP::request & req, Torch::HTTP::response & res) {
        res.send(std::string("<h1>Bank Account</h1>") + req.url() + "</p>");
	}

	void redirect(const Torch::HTTP::request & req, Torch::HTTP::response & res) {
		res.redirect("http://nasa.gov", 301);
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
		app.get("/account", &Routes::account);
		app.get("/redirectme", &Routes::redirect);

        log::inst().close_logs();

        app.listen(8080);

	} catch (std::exception & e) {
		log::inst().error(e.what());
	}

	return 0;
}
