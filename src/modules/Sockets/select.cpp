//
// Copyright (C) 2012 Claudiu Tasca <clawgm [at] gmail.com>, Marius Petcu <marius
// [at] porkholt.org>, Nandor Licker <licker.nandor [at] gmail.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

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
