#ifndef __SOCKETS_SELECT_H__
#define __SOCKETS_SELECT_H__

#include <map>

namespace Torch {
    namespace Sockets {
        class Socket;

        struct SelectSet {
            std::map<Socket*, std::pair<int, int> > data;
            
            enum flags {
                read = 1<<0,
                write = 1<<1,
                error = 1<<2
            };

            int count() { return data.size(); }
            void insertSocket(Socket * s, int flags) { data[s] = std::make_pair(flags, 0); }
            void removeSocket(Socket * s) { data.erase(s); }
            bool canRead(Socket * s) { return (data[s].second & read) != 0; } 
            bool canWrite(Socket * s) { return (data[s].second & write) != 0; } 
            bool hasError(Socket * s) { return (data[s].second & error) != 0; } 
        };

        void select(SelectSet & s, int timeout);
    }
}

#endif
