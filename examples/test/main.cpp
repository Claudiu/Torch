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
    res.send(std::string("<h1>Bank Account</h1>") + req.url() + " and something like " + req.body("name") + "</p>");
	}

	void cookie(const Request & req, Response & res) {
		res.setCookie("username", "claudiu");
		
		std::string cookie = req.getCookie("username");

		Log::inst().notice("Found cookie %s.", cookie.c_str());
		res.send(200, cookie);
	}

	void redirect(const Request & req, Response & res) {
		res.redirect("http://feralhost.in/");
	}
};

// This is one of the worst hacks ever written by me
// Why not do something like"Server running on port" if (!signal_already_bound(SIGINT)) signal(SIGINT, sigint);
// in app.listen()?
Torch::HTTP::Application app;

int main(int argc, char const *argv[])
{
	try {
		Log::inst().openLogs();

		app.staticDir = "static";

		app.get("/cookie", &Routes::cookie);	
		app.get("/", &Routes::index);
		app.post("/account", &Routes::account);
		app.put("/account", &Routes::account);
		app.remove("/account", &Routes::account);

		app.put("/redirectme", &Routes::redirect);

    Log::inst().closeLogs();

    app.listen(PORT);

	} catch (std::exception & e) {
		Log::inst().error(e.what());
	}

	return 0;
}
