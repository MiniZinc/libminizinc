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
        /** *** *** *** Phase 1 main fields *** *** *** **/

        map<string, Var>        variables={};           // Mapping variable name

        vector<AlgebraicCons>   algcons={};             // Algebraic constraint


        /** *** *** *** Phase 2 main fields *** *** *** **/

        // Ordering of variables according to "hooking your solver"
        /*  Meaning of the names (total, then by appearance order in the tables below)
                n_var               total number of variables
                nlvc                number of variables appearing nonlinearly in constraints
                nlvo                number of variables appearing nonlinearly in objectives
                nwv                 number of linear arcs
                niv                 number of "other" integer variables
                nbv                 number of binary variables


            Order of variables:
            Category            Count
            --- --- --- --- |   --- --- --- --- ---
            nonlinear           max(nlvc, nlvo)                                 // See below for order on non linear variables
            linear arcs         nwv                                             // Not implemented
            other linear        n_var − (max {nlvc, nlvo} + niv + nbv + nwv)    // Linear Continuous
            binary              nbv                                             // Booleans
            other integer       niv                                             // Linear Integer



            Order of non linear variables (see 'nonlinear' above)
            Meaning of the names:
                nlvb            number of variables appearing nonlinearly in both constraints and objectives
                nlvbi           number of integer variables appearing nonlinearly in both constraints and objectives
                nlvc            number of variables appearing nonlinearly in constraints
                nlvci           number of integer variables appearing nonlinearly in constraints **only**
                nlvo            number of number of variables appearing nonlinearly in objectives
                nlvoi           number of integer variables appearing nonlinearly in onjectives **only**

            Category                                                Count
            --- --- --- --- --- --- --- --- --- --- --- --- --- |   --- --- --- --- ---
            Continuous in BOTH an objective AND a constraint    |   nlvb - nlvbi
            Integer, in BOTH an objective AND a constraint      |   nlvbi
            Continuous, in constraints only                     |   nlvc − (nlvb + nlvci)
            Integer, in constraints only                        |   nlvci
            Continous, in objectives only                       |   nlvo − (nlvc + nlvoi)
            Integer, in objectives only                         |   nlvoi
        */

        // Non Linear Continuous Variables in BOTH an objective and a constraint
        vector<string>  vname_nlcv_both = {};
        // Non Linear Integer Variables in BOTH an objective and a constraint
        vector<string>  vname_nliv_both = {};
        // Non Linear Continuous Variables in CONStraints only
        vector<string>  vname_nlcv_cons = {};
        // Non Linear Integer Variables in CONStraints only
        vector<string>  vname_nliv_cons = {};
        // Non Linear Continuous Variables in OBJectives only
        vector<string>  vname_nlcv_obj = {};
        // Non Linear Integer Variables in OBJectives only
        vector<string>  vname_nliv_obj = {};
        // Linear Continuous Variables (ALL of them)
        vector<string>  vname_lcv_all = {};
        // Binary Variables (ALL of them)
        vector<string>  vname_bv_all = {};
        // Linear Integer Variables (ALL of them)
        vector<string>  vname_liv_all = {};

        // End of phase 2: this vector contains all the variables names, in a sorted order (also gives the retro mapping index -> name)
        vector<string>  vnames={};       











        // The header fields. TODO: create the header based on the above data
        NLHeader                header={};              // 0-init the header




        // Ordering of constraints according to "hooking your solver"
        /* Meaning of te names:
            n_con               total number of constraints
            --- --- --- --- |   --- --- --- --- ---
        */



        /** *** *** *** Segments *** *** *** **/
        NLS_Bound           b_segment;
        NLS_Range           r_segment;
        NLS_OSeg            o_segment;
        vector<NLS_CSeg>    c_segments = {};                // Constraint segments
        vector<NLS_JSeg>    j_segments = {};                // Constraint segments: linear part
        vector<NLS_LSeg>    l_segments = {};                // Logical constraint segments

        /** *** *** *** Other info *** *** *** **/
        bool is_optimisation = false;                       // Is an optimisation problem ?
        
        /** *** *** *** Constructor *** *** *** **/
        NLFile():
            b_segment(this), r_segment(this), o_segment(this) {}

        
        /** *** *** *** Printable Interface *** *** *** **/

        ostream& print_on( ostream& o ) const override ;

        /** *** *** *** Helpers *** *** *** **/

        static string get_vname(const VarDecl &vd);

        static ASTExprVec<Expression> get_vec(const Expression* e);

        NLS_JSeg make_jseg_int(int considx, const ASTExprVec<Expression> &coeffs, const ASTExprVec<Expression> &vars);

        NLS_JSeg make_jseg_fp(int considx, const ASTExprVec<Expression> &coeffs, const ASTExprVec<Expression> &vars);
        

        /** *** *** *** Solve analysis *** *** *** **/
        void analyse_solve(SolveI::SolveType st, const Expression* e);

        /** *** *** *** Variable declaration methods *** *** *** **/

        void analyse_vdecl(const VarDecl &vd, const TypeInst &ti, const Expression &rhs);

        void vdecl_integer(const string& name, const IntSetVal* isv, bool to_report);

        void vdecl_fp(const string& name, const FloatSetVal* fsv, bool to_report);

        /** *** *** *** Constraints methods *** *** *** **/

        void analyse_constraint(const Call& c);

        /** *** *** *** Integer Constraint methods *** *** *** **/

        void consint_lin_eq(const Call& c);
        void consint_lin_le(const Call& c);
        void consint_lin_ne(const Call& c);

        void consint_times(const Call& c);
        void consint_div(const Call& c);
        void consint_mod(const Call& c);

        void consint_le(const Call& c);
        void consint_eq(const Call& c);
        void consint_ne(const Call& c);

        /** *** *** *** Floating Point Constraint methods *** *** *** **/
        void consfp_lin_eq(const Call& c);
        void consfp_lin_le(const Call& c);
        void consfp_lin_lt(const Call& c);
        void consfp_lin_ne(const Call& c);
        void consfp_plus(const Call& c);
        void consfp_minus(const Call& c);
        void consfp_times(const Call& c);
        void consfp_div(const Call& c);
        void consfp_mod(const Call& c);
        void consfp_lt(const Call& c);
        void consfp_le(const Call& c);
        void consfp_eq(const Call& c);
        void consfp_ne(const Call& c);

        

    };

} // End of NameSpace MiniZinc

#endif