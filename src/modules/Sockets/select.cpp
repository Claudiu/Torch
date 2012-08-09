#include <torch/sockets.hpp>
#include <poll.h>

using namespace Torch;
using namespace Torch::Sockets;

void Torch::Sockets::select(SelectSet & s, int timeout)
{
    int n = s.count();
    struct pollfd * pf = new struct pollfd[n];
    int j = 0;
    for (std::map<Socket*, std::pair<int, int> >::iterator i = s.data.begin(); i!=s.data.end(); i++, j++)
    {
        pf[j].fd = i->first->fileDescriptor();
        int mask = 0;
        int um  = i->second.first;
        if (um & SelectSet::read)
            mask |= POLLIN;
        if (um & SelectSet::write)
            mask |= POLLOUT;
        if (um & SelectSet::error)
            mask |= POLLERR;
        pf[j].events = mask;
    }
    poll(pf, n, timeout);
    j = 0;
    for (std::map<Socket*, std::pair<int, int> >::iterator i = s.data.begin(); i!=s.data.end(); i++, j++)
    {
        int mask = pf[j].revents;
        int um = 0;
        if (mask & POLLIN)
            um |= SelectSet::read;
        if (mask & POLLOUT)
            um |= SelectSet::write;
        if (mask & POLLERR)
            um |= SelectSet::error;
        i->second.second = um;
    }
    delete[] pf;
}
