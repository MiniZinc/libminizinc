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
#include <minizinc/solvers/nl/nl_expressions.hh>

using namespace std;

// This files declare data-structure describing the various components of a nl files.
// A nl files is composed of two main parts: a header and a list of segments.
// The header contains statistics and meta information about the model.
// The segments allow to describe the model, i.e. the variables, the constraints and objectives.
// The order of the segments and, when relevant, the order of their content
// (e.g. a segment declaring a list of variables) matters.

namespace MiniZinc {

    // --- --- --- NL Files
    class NLFile: public Printable {

        public:
        /** *** *** *** Fields *** *** *** **/
        
        NLHeader                header={};              // 0-init the header

        map<string, Var>        variables={};           // Mapping variable name
        vector<string>          name_vars={};           // Retro Mapping index -> name

        vector<AlgebraicCons>   algcons={};             // Algebraic constraint

        /** *** *** *** Segments *** *** *** **/
        NLS_Bound           b_segment;
        NLS_Range           r_segment;
        NLS_OSeg            o_segment;
        vector<NLS_CSeg>    c_segments = {};                // Constraint segments
        vector<NLS_JSeg>    j_segments = {};                // Constraint segments: linear part
        
        /** *** *** *** Constructor *** *** *** **/
        NLFile():
            b_segment(this), r_segment(this), o_segment(this) {}

        
        /** *** *** *** Printable Interface *** *** *** **/

        ostream& print_on( ostream& o ) const override ;

        /** *** *** *** Helpers *** *** *** **/

        static string get_vname(const VarDecl &vd);

        static ASTExprVec<Expression> get_vec(const Expression* e);

        /** *** *** *** Solve analysis *** *** *** **/
        void analyse_solve(SolveI::SolveType st, const Expression* e);

        /** *** *** *** Variable declaration methods *** *** *** **/

        void analyse_vdecl(const VarDecl &vd, const TypeInst &ti, const Expression &rhs);

        void vdecl_integer(const string& name, const IntSetVal* isv);

        void vdecl_fp(const string& name, const FloatSetVal* fsv);

        /** *** *** *** Constraints methods *** *** *** **/

        void analyse_constraint(const Call& c);

        /** *** *** *** Integer Constraint methods *** *** *** **/

        void consint_lin_eq(const Call& c);
        void consint_lin_le(const Call& c);
        
        void consint_le(const Call& c);

    };

} // End of NameSpace MiniZinc

#endif