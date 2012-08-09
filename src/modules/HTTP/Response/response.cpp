#include <torch/HTTP/Response/response.hpp>
#include <torch/sockets.hpp>

Torch::HTTP::response::response(socket * sock) {
	this->sock = sock;
	this->setServerName("Torch");
	this->setLocation("");
}

void Torch::HTTP::response::redirect(std::string to, short code) {
	this->setLocation(to);
	this->send(code);
}


void Torch::HTTP::response::send(std::string what) {
	this->send(HTTP_OK, what);
}

void Torch::HTTP::response::send(short code, std::string what) {
	std::stringstream temp;

    static std::map<short, std::string> * codes = NULL;
    if (!codes)
    {
        codes = new std::map<short, std::string>;
        (*codes)[200] = "OK";
        (*codes)[307] = "Temporary Redirect";
        (*codes)[404] = "Not Found";
        (*codes)[418] = "I'm a teapot";

    }

	temp << "HTTP/1.1 "<<code<<" "<<(*codes)[code]<<"\n";
	
	if(what.length() != 0)
		temp << "Content-Type: text/html\n";

	if(!this->getLocation().empty())
		temp << "Location: " << this->getLocation() << "\n";

	if(!this->getServerName().empty())
		temp << "Server: " << this->getServerName() << "\n";
	
	temp << "Content-Length: " << what.length() << "\n";
	temp << "Connection: close\n";
	temp << "\n";
	temp << what;

	std::string t = temp.str();
	const char * kitty = t.c_str();

	size_t kitty_tail_length = strlen(kitty); 
	uint8_t * rez = new uint8_t[kitty_tail_length];

	memcpy(rez, kitty, kitty_tail_length);

	log::inst().notice("Sending HTTP response back.");

	this->sock->queue_for_writing(rez, kitty_tail_length);
}
