#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <torch/http.hpp>
#include <torch/log.hpp>

const short PORT = 5314;

using namespace Torch;
using namespace Torch::HTTP;

namespace Routes {
	void index(const Request & req, Response & res) {
        res.send(std::string("<h1>Torch is fucking awesome</h1><p>This is the first html test in history</p> <p>") + req.url() + "</p>");
	}

	void account(const Request & req, Response & res) {
		res.setHeader("Content-Type", "application/json");
        res.send(std::string("<h1>Bank Account</h1>") + req.url() + " and something like " + req.body() + "</p>");
	}

	void redirect(const Request & req, Response & res) {
		res.redirect("http://feralhost.in/");
	}
};

// This is one of the worst hacks ever written by me
// Why not do something like"Server running on port" if (!signal_already_bound(SIGINT)) signal(SIGINT, sigint);
// in app.listen()?
Torch::HTTP::Application app;
void sigint(int)
{
	app.close();
	printf ("The torch was extinguished :(\n\n");
	exit(0);
}

int main(int argc, char const *argv[])
{
	try {
		Log::inst().openLogs();

		signal(SIGINT, sigint);

		app.staticDir = "static";

		app.get("/", &Routes::index);
		app.get("/account", &Routes::account);
		app.put("/redirectme", &Routes::redirect);

    Log::inst().closeLogs();

    app.listen(PORT);

	} catch (std::exception & e) {
		Log::inst().error(e.what());
	}

	return 0;
}
