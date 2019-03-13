#ifndef __MINIZINC_NL_PRINTABLE_HH__
#define __MINIZINC_NL_PRINTABLE_HH__

#include <ostream>
using namespace std;

namespace MiniZinc {
    
    class NLFile;

    /** Printable interface.
     *  Objects implementing this interface can output themselves as ASCII text in a ostream given a reference on the NL_FILE they are living in.
     *  This reference may be used for several purposes, such as resolving a name into an indexes,
     *  or gathering some information prior to printing.
     */ 
    class Printable {
        public:
        virtual ostream& print_on( ostream& o, const NLFile& nl_file) const =0;
    };

}

#endif