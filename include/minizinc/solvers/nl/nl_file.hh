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

    /** Variable Record
     *  The variable record contains the name, index and type (is_integer -> integer or floating point)
     *  of the a variable. It also contains the variable bound as describe into nl_segment.hh.
     *  Note that in flatzinc, the bound are eigher absent ot present on both side.
     *  In contrast, nl support partial bounds.
     *  Partial bounds in flatzinc are expressed through contraints: while analysing constraints, we reconstruct
     *  partial bound at the variable level.
     */
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
        /** *** *** *** Fields *** *** *** **/
        
        NLHeader                header={};              // 0-init the header
        map<string, Var>        variables={};           // Mapping variable name
        vector<string>          name_vars={};           // Retro Mapping index -> name
        
        /** *** *** *** Printable Interface *** *** *** **/

        ostream& print_on( ostream& o ) const override ;

        /** *** *** *** Helpers *** *** *** **/

        static string get_vname(const VarDecl &vd);

        /** *** *** Variable declaration methods *** *** *** **/

        void analyse_vdecl(const VarDecl &vd, const TypeInst &ti, const Expression &rhs);

        void vdecl_integer(const string& name, const IntSetVal* isv);

        void vdecl_fp(const string& name, const FloatSetVal* fsv);

        /** *** *** Constraints methods *** *** *** **/

        void analyse_constraint(const Call& c);

        /** *** *** Integer Constraint methods *** *** *** **/

        void consint_lin_eq(const Call& c);
        void consint_le(const Call& c);

    };

} // End of NameSpace MiniZinc

#endif