
#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#include <string>

namespace Torch
{
    class string_exception : public std::exception
    {
        private:
        std::string s;
        
        public:
        string_exception(const std::string& str) throw() : s(str) {};
        string_exception(const string_exception & o) throw() : s(o.s) {};
        virtual ~string_exception() throw() { }

        const char * what() const throw() { return s.c_str(); }

        string_exception& operator << (int i);
        string_exception& operator << (const std::string& s);
    };

}

#endif /*__EXCEPTION_HPP__*/