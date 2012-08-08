#ifndef __SOCKETS_SELECT_H__
#define __SOCKETS_SELECT_H__

#include <map>

namespace Torch {
    namespace Sockets {
        class socket;

        struct select_struct {
            std::map<socket*, std::pair<int, int> > data;
            
            enum flags {
                read = 1<<0,
                write = 1<<1,
                error = 1<<2
            };

            int count() { return data.size(); }
            void insert_socket(socket * s, int flags) { data[s] = std::make_pair(flags, 0); }
            void remove_socket(socket * s) { data.erase(s); }
            bool can_read(socket * s) { return (data[s].second & read) != 0; } 
            bool can_write(socket * s) { return (data[s].second & write) != 0; } 
            bool has_error(socket * s) { return (data[s].second & error) != 0; } 
        };

        void select(select_struct & s, int timeout);
    }
}

#endif
