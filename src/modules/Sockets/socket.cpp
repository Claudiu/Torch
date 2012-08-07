#include <torch/sockets.hpp>
#include <torch/logs.hpp>

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>

#include <sstream>

using namespace Torch::Sockets;

const char * socket_exception::what() const throw() {
    return strerror(err);
}

bool socket_exception::is_eagain() const {
    return ((err == EAGAIN) || (err == EWOULDBLOCK));
}

socket::socket(int filedes) : fd(filedes)
{
    
}

socket::~socket()
{
    close(fd);
    for (std::list< std::pair<uint8_t *, size_t> >::iterator i = wq.begin(); i!=wq.end(); i++) //the reason why C++11's auto keyword would be awesome
        delete[] i->first;
}

void socket::set_blocking(bool blocking)
{
    if (blocking)
		fcntl(fd, F_SETFL, 0);
	else
		fcntl(fd, F_SETFL, O_NONBLOCK);
}

bool socket::blocking()
{
    return (!(fcntl(fd, F_GETFL) | O_NONBLOCK));
}

ssize_t socket::write(uint8_t * buf, size_t size)
{
    ssize_t r = send(fd, buf, size, 0);
    if (r == -1)
        throw socket_exception(errno);
    return r;
}

ssize_t socket::read(uint8_t * buf, size_t size)
{
    ssize_t r = recv(fd, buf, size, 0);
    if (r == -1)
        throw socket_exception(errno);
    return r;
}

bool socket::can_read()
{
    struct pollfd p;
    p.fd = fd;
    p.events = POLLIN;
    p.revents = 0;
    poll(&p, 1, 0);
    return (p.revents | POLLIN) != 0;
}

bool socket::can_write()
{
    struct pollfd p;
    p.fd = fd;
    p.events = POLLOUT;
    p.revents = 0;
    poll(&p, 1, 0);
    return (p.revents | POLLOUT) != 0;
}

bool socket::error()
{
    struct pollfd p;
    p.fd = fd;
    p.events = POLLERR;
    p.revents = 0;
    poll(&p, 1, 0);
    return (p.revents | POLLERR) != 0;
}

bool socket::can_accept()
{
    return can_read();
}

ssize_t socket::read_peek(uint8_t * buf, size_t size)
{
   ssize_t r = recv(fd, buf, size, MSG_PEEK);
   if (r == -1)
       throw socket_exception(errno);
   return r;
}

Torch::Sockets::socket * socket::accept()
{
    struct sockaddr_storage ai;
    socklen_t s = sizeof(ai);
    int res = ::accept(fd, (struct sockaddr*)&ai, &s);
    if (res == -1)
        throw socket_exception(errno);
    return new socket(res);
}

void socket::queue_for_writing(uint8_t * buf, size_t size)
{
    wq.push_back(std::make_pair(buf, size));
}

void socket::write_from_queue()
{
    try {
        while (!wq.empty())
        {
            uint8_t * b = wq.front().first;
            size_t s = wq.front().second;

            b = b+woff;
            s-= woff;

            size_t sr = write(b, s);
            woff += sr;
            s-= sr;

            if (!s)
            {
                delete[] wq.front().first;
                wq.pop_front();
                woff = 0;
            } else break;
        }
    } catch(socket_exception ex) {
        if (!ex.is_eagain())
            throw ex;
    }
}

std::vector<Torch::Sockets::socket*> socket::tcp_listeners_all_interfaces(short port)
{
    std::vector<socket*> v;
    

    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::stringstream oss;
    oss<<port;

    if ((rv = getaddrinfo(NULL, oss.str().c_str(), &hints, &servinfo)) != 0) {
        throw Torch::string_exception(gai_strerror(rv));
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        int sockfd;
        if (((sockfd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol)),sockfd) == -1) {
            continue;
	        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        if (listen(sockfd, SOMAXCONN))
        {
            close(sockfd);
            continue;
        }
        
        v.push_back(new socket(sockfd));
    }

    freeaddrinfo(servinfo);

    return v;
}
