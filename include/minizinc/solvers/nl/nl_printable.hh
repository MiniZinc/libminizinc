#ifndef __MINIZINC_NL_PRINTABLE_HH__
#define __MINIZINC_NL_PRINTABLE_HH__

#include <ostream>
using namespace std;

namespace MiniZinc {
    
    // --- --- --- Interface
    // Our components (header and segments) are "printable":
    // they can output themselves as ASCII text in a ostream.
    class Printable {
        public:
        virtual ostream& print_on( ostream& o ) const =0;
    };

    template<class Char, class Traits>
    std::basic_ostream<Char,Traits>&
    operator <<(std::basic_ostream<Char,Traits>& o, const Printable& p){
        p.print_on(o);
        return o;
    }
}

#endif