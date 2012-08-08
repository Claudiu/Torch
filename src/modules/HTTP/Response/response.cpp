#include <torch/HTTP/Response/response.hpp>
#include <torch/sockets.hpp>

Torch::HTTP::response::response(socket * sock) {
	this->sock = sock;
}

void Torch::HTTP::response::send(std::string what, short code) {
	std::stringstream temp;

    static std::map<short, std::string> * codes = NULL;
    if (!codes)
    {
        codes = new std::map<short, std::string>;
        (*codes)[200] = "OK";
        (*codes)[404] = "Not Found";
        (*codes)[418] = "I'm a teapot";
    }

	temp << "HTTP/1.1 "<<code<<" "<<(*codes)[code]<<"\n";
	temp << "Content-Type: text/html\n";
	temp << "Content-Length: " << what.length() << "\n";
	temp << "Connection: close\n";
	temp << "\n";
	temp << what;

	std::string t = temp.str();
	const char * kitty = t.c_str();

	size_t kitty_tail_length = strlen(kitty); 
	uint8_t * rez = new uint8_t[kitty_tail_length];

	memcpy(rez, kitty, kitty_tail_length);

	Torch::toLog("Sending HTTP response back.");

	this->sock->queue_for_writing(rez, kitty_tail_length);
}
