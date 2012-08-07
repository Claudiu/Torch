#include <torch/sockets.hpp>
#include <poll.h>

using namespace Torch;
using namespace Torch::Sockets;

void Torch::Sockets::select(select_struct & s, int timeout)
{
    int n = s.count();
    struct pollfd * pf = new struct pollfd[n];
    int j = 0;
    for (std::map<socket*, std::pair<int, int> >::iterator i = s.data.begin(); i!=s.data.end(); i++, j++)
    {
        pf[j].fd = i->first->file_descriptor();
        int mask = 0;
        int um  = i->second.first;
        if (um & select_struct::read)
            mask |= POLLIN;
        if (um & select_struct::write)
            mask |= POLLOUT;
        if (um & select_struct::error)
            mask |= POLLERR;
        pf[j].events = mask;
    }
    poll(pf, n, timeout);
    j = 0;
    for (std::map<socket*, std::pair<int, int> >::iterator i = s.data.begin(); i!=s.data.end(); i++, j++)
    {
        int mask = pf[j].revents;
        int um = 0;
        if (mask & POLLIN)
            um |= select_struct::read;
        if (mask & POLLOUT)
            um |= select_struct::write;
        if (mask & POLLERR)
            um |= select_struct::error;
        i->second.second = um;
    }
    delete[] pf;
}
