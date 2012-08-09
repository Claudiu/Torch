
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/Socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>

#include <sstream>

#include <torch/Sockets.hpp>
#include <torch/Log.hpp>
#include <torch/util.hpp>


using namespace Torch::Sockets;

const char * SocketException::what() const throw() {
    return strerror(err);
}

bool SocketException::isEAGAIN() const {
    return ((err == EAGAIN) || (err == EWOULDBLOCK));
}

Socket::Socket(int filedes) : fd(filedes), woff(0)
{
    
}

Socket::~Socket()
{
    close(fd);
    for (std::list< std::pair<uint8_t *, size_t> >::iterator i = wq.begin(); i!=wq.end(); i++) //the reason why C++11's auto keyword would be awesome
        delete[] i->first;
}

void Socket::setBlocking(bool blocking)
{
    if (blocking)
		fcntl(fd, F_SETFL, 0);
	else
		fcntl(fd, F_SETFL, O_NONBLOCK);
}

bool Socket::blocking()
{
    return (!(fcntl(fd, F_GETFL) & O_NONBLOCK));
}

ssize_t Socket::write(uint8_t * buf, size_t size)
{
    ssize_t r = send(fd, buf, size, 0);
    if (r == -1)
        throw SocketException(errno);
    return r;
}

ssize_t Socket::read(uint8_t * buf, size_t size)
{
    ssize_t r = recv(fd, buf, size, 0);
    if (r == -1)
        throw SocketException(errno);
    return r;
}

bool Socket::canRead()
{
    struct pollfd p;
    p.fd = fd;
    p.events = POLLIN;
    p.revents = 0;
    poll(&p, 1, 0);
    return (p.revents & POLLIN) != 0;
}

bool Socket::canWrite()
{
    struct pollfd p;
    p.fd = fd;
    p.events = POLLOUT;
    p.revents = 0;
    poll(&p, 1, 0);
    return (p.revents & POLLOUT) != 0;
}

bool Socket::error()
{
    struct pollfd p;
    p.fd = fd;
    p.events = POLLERR;
    p.revents = 0;
    poll(&p, 1, 0);
    return (p.revents & POLLERR) != 0;
}

bool Socket::canAccept()
{
    return canRead();
}

ssize_t Socket::readPeek(uint8_t * buf, size_t size)
{
   ssize_t r = recv(fd, buf, size, MSG_PEEK);
   if (r == -1)
       throw SocketException(errno);
   return r;
}

Torch::Sockets::Socket * Socket::accept()
{
    struct sockaddr_storage ai;
    socklen_t s = sizeof(ai);
    int res = ::accept(fd, (struct sockaddr*)&ai, &s);
    if (res == -1)
        throw SocketException(errno);
    return new Socket(res);
}

void Socket::queueForWriting(uint8_t * buf, size_t size)
{
    wq.push_back(std::make_pair(buf, size));
}

void Socket::writeFromQueue()
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
    } catch(SocketException ex) {
        if (!ex.isEAGAIN())
            throw ex;
    }
}

std::vector<Torch::Sockets::Socket*> Socket::tcpListenersAllInterfaces(short port)
{
    std::vector<Socket*> v;
    

    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::stringstream oss;
    oss<<port;

    if ((rv = getaddrinfo(NULL, oss.str().c_str(), &hints, &servinfo)) != 0) {
        throw Torch::StringException(gai_strerror(rv));
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        int sockfd;
        if (((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)),sockfd) == -1) {
            Log::inst().warn("socket() failed \"%s\"", strerror(errno));
            continue;
	        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            Log::inst().warn("bind() failed \"%s\"", strerror(errno));
            close(sockfd);
            continue;
        }

        if (listen(sockfd, SOMAXCONN))
        {
            Log::inst().warn("listen() failed \"%s\"", strerror(errno));
            close(sockfd);
            continue;
        }
        
        v.push_back(new Socket(sockfd));
    }

    freeaddrinfo(servinfo);

    return v;
}
