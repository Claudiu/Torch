
#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#include <string>

namespace Torch
{
    class StringException : public std::exception
    {
        private:
        std::string s;
        
        public:
        StringException(const std::string& str) throw() : s(str) {};
        StringException(const StringException & o) throw() : s(o.s) {};
        virtual ~StringException() throw() { }

        const char * what() const throw() { return s.c_str(); }

        StringException & operator << (int i);
        StringException & operator << (const std::string& s);
    };

}

#endif /*__EXCEPTION_HPP__*/
