#include <iostream>
#include "Modules/HTTP/application.hpp"

int main(int argc, char const *argv[])
{
	Torch::HTTP::application app;
	
	app.get("STDIO");
	
	app.listen(8080);

	return 0;
}