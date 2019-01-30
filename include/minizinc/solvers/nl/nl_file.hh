/**
 * Describe the structure of a NL file.
 * Main author: Matthieu Herrmann, Monash University, Melbourne, Australia. 2019
 **/

#ifndef __MINIZINC_NL_FILE_HH__
#define __MINIZINC_NL_FILE_HH__

#include <string>
#include <ostream>
#include <map>
#include <set>

#include <minizinc/astvec.hh>
#include <minizinc/ast.hh>
#include <minizinc/solvers/nl/nl_printable.hh>
#include <minizinc/solvers/nl/nl_header.hh>
#include <minizinc/solvers/nl/nl_segments.hh>

using namespace std;

// This files declare data-structure describing the various components of a nl files.
// A nl files is composed of two main parts: a header and a list of segments.
// The header contains statistics and meta information about the model.
// The segments allow to describe the model, i.e. the variables, the constraints and objectives.
// The order of the segments and, when relevant, the order of their content
// (e.g. a segment declaring a list of variables) matters.

namespace MiniZinc {

    class NLFile;

    // --- --- --- Variable record
    class Var {
        public:
        string const*   name;
        int             index;
        bool            is_integer;
        NLS_BoundItem   bound;

        public:
        Var() = default;
        Var(const string& name, int index, bool is_integer, NLS_BoundItem bound):
            name(&name), index(index), is_integer(is_integer), bound(bound){}
    };


    // --- --- --- NL Files
    class NLFile: public Printable {

        public:

        // --- --- --- --- --- --- --- --- --- Fields
        NLHeader                header={};              // 0-init the header
        map<string, Var>        variables={};           // Mapping variable name
        vector<string>          name_vars={};           // Retro Mapping index -> name
        

        // --- --- --- --- --- --- --- --- --- Methods
        
        ostream& print_on( ostream& o ) const override ;

        // --- --- --- --- --- --- VDECL
        void vdecl_integer(const string& name, const IntSetVal* isv);

        void vdecl_fp(const string& name, const FloatSetVal* fsv);

        void add_vdecl_enum();
            
        void add_vdecl_tystr();

        // void add_vdecl(const string& name, const Type& type, const Expression* domain);

    };

} // End of NameSpace MiniZinc

#endif