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
#include <minizinc/solvers/nl/nl_components.hh>

using namespace std;

// This files declare data-structure describing the various components of a nl files.
// A nl files is composed of two main parts: a header and a list of segments.
// The header contains statistics and meta information about the model.
// The segments allow to describe the model, i.e. the variables, the constraints and objectives.
// The order of the segments and, when relevant, the order of their content
// (e.g. a segment declaring a list of variables) matters.

/** NL File.
 *  Good to know:
 *      * We use string as variable unique identifier.
 *        Given a MZN variable declaration (can be obtain from a MZN variable), the 'get_vname' helper produces the string.
 *      * In our case, we only have one 'solve' per file.
 *      * NL file use double everywhere. Hence, even with dealing with integer variable, we store the information with double.
 * 
 */

namespace MiniZinc {

    // --- --- --- NL Files
    class NLFile {

        public:

        /* *** *** *** Helpers *** *** *** */

        /** Create a string representing the name (and unique identifier) of a variable from a variable declaration. */
        static string get_vname(const VarDecl &vd);

        /** Create a string representing the name (and unique identifier) of a constraint from a specific call expression. */
        static string get_cname(const Call& c);

        /** Extract a vector of expression from an expression. Used when working with arrays. */
        static ASTExprVec<Expression> get_vec(const Expression* e);

        /** Create a vector of double from a vector containing Expression being integer literal IntLit. */
        static vector<double> from_vec_int(const ASTExprVec<Expression>& v_int);

        /** Create a vector of double from a vector containing Expression being float literal FloatLit. */
        static vector<double> from_vec_fp(const ASTExprVec<Expression>& v_fp);

        /** Create a vector of variable names from a vector containing Expression being identifier Id. */
        static vector<string> from_vec_id(const ASTExprVec<Expression>& v_id);



        /* *** *** *** Phase 0: constructor data *** *** *** */
        string problem_name;


        /* *** *** *** Phase 1: collecting data from MZN *** *** *** */

        // Variables collection, identified by name
        map<string, NLVar> variables={};

        // Algebraic constraints collection, identified by name
        map<string, NLAlgCons> constraints={};

        // Logical constraints do not need ordering:
        vector<NLLogicalCons> logical_constraints={};


        // Objective field. Only one, so we do not need ordering.
        NLSeg_O segment_O = {};


        /** Add a solve goal in the NL File. In our case, we can only have one and only one solve goal. */
        void add_solve(SolveI::SolveType st, const Expression* e);


        /** Add a variable declaration in the NL File.
         *  This function pre-analyse the declaration VarDecl, then delegate to add_vdecl_integer or add_vdecl_fp.
         *  Analyse a variable declaration 'vd' of type 'ti' with an 'rhs'.
         *  The variable declaration gives us access to the variable name while the type allows us to discriminate between integer,
         *  floating point value and arrays.
         *  Array are ignored (not declared): if we encouter an array in a constraint, we can find the array through the variable (ot it is a litteral).
         *  Notes:  - We use -Glinear, so we do not have boolean.
         *          - This will change TODO keep checking comment and code consistency.
         *
         * RHS is for arrays: it contains the definition of the array.
         * 
         * The type also gives us the domain, which can be:
         *  NULL:       no restriction over the variable
         *  SetLit:     Gives us a lower and upper bound
         *  If a variable is bounded only on one side, then the domain is NULL and the bound is expressed through a constraint.
         */
        void add_vdecl(const VarDecl &vd, const TypeInst &ti, const Expression &rhs);

        /** Add an integer variable declaration to the NL File. */
        void add_vdecl_integer(const string& name, const IntSetVal* isv, bool to_report);

        /** Add a floating point variable declaration to the NL File. */
        void add_vdecl_fp(const string& name, const FloatSetVal* fsv, bool to_report);



        /** Add a constraint to the NL File.
         * This method is a dispatcher for all the other constraints methods below. */
        void analyse_constraint(const Call& c);

        /* *** *** *** Integer Constraint methods *** *** *** */

        void consint_lin_eq(const Call& c);
        void consint_lin_le(const Call& c);
        void consint_lin_ne(const Call& c);

        void consint_times(const Call& c);
        void consint_div(const Call& c);
        void consint_mod(const Call& c);

        void consint_le(const Call& c);
        void consint_eq(const Call& c);
        void consint_ne(const Call& c);

        /* *** *** *** Floating Point Constraint methods *** *** *** */

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












        /* *** *** *** Phase 2: processing *** *** *** */

        void phase_2();

        // Ordering of variables according to "hooking your solver"
        /*  Meaning of the names (total, then by appearance order in the tables below)
                n_var               total number of variables
                nlvc                number of variables appearing nonlinearly in constraints
                nlvo                number of variables appearing nonlinearly in objectives
                nwv                 number of linear arcs
                niv                 number of "other" integer variables
                nbv                 number of binary variables


            Order of variables (yes, the way things are counted is CRAZY!!!!)
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
                nlvo            number of variables appearing nonlinearly in objectives
                nlvoi           number of integer variables appearing nonlinearly in objectives **only**

            Category                                                Count
            --- --- --- --- --- --- --- --- --- --- --- --- --- |   --- --- --- --- ---
            Continuous in BOTH an objective AND a constraint    |   nlvb - nlvbi
            Integer, in BOTH an objective AND a constraint      |   nlvbi
            Continuous, in constraints only                     |   nlvc − (nlvb + nlvci)
            Integer, in constraints only                        |   nlvci
            Continous, in objectives only                       |   nlvo − (nlvc + nlvoi)
            Integer, in objectives only                         |   nlvoi
        */

        /** Non Linear Continuous Variables in BOTH an objective and a constraint. */
        vector<string>  vname_nlcv_both = {};

        /** Non Linear Integer Variables in BOTH an objective and a constraint. */
        vector<string>  vname_nliv_both = {};

        /** Non Linear Continuous Variables in CONStraints only. */
        vector<string>  vname_nlcv_cons = {};

        /** Non Linear Integer Variables in CONStraints only. */
        vector<string>  vname_nliv_cons = {};

        /** Non Linear Continuous Variables in OBJectives only. */
        vector<string>  vname_nlcv_obj = {};

        /** Non Linear Integer Variables in OBJectives only. */
        vector<string>  vname_nliv_obj = {};

        /** Linear arcs. (Network not implemented) */
        vector<string>  vname_larc_all = {};

        /** Linear Continuous Variables (ALL of them). */
        vector<string>  vname_lcv_all = {};

        /** Binary Variables (ALL of them). */
        vector<string>  vname_bv_all = {};

        /** Linear Integer Variables (ALL of them). */
        vector<string>  vname_liv_all = {};

        // End of phase 2: this vector contains all the variables names, in a sorted order (also gives the retro mapping index -> name)

        /** Mapping variable index -> variable name */
        vector<string>  vnames={};

        /** Mapping variable name -> variable index */
        map<string, int> variable_indexes={};



        // Ordering of constraints according to "hooking your solver"
        /*  Meaning of the names:
                n_con       Total number of constraint
                nlc         Number of nonlinear general constraint, including network constraint
                nlnc        Number of nonlinear network constraint
                lnc         Number of linear network constraint
            
            Order of constraints:
            Category                Count
            --- --- --- --- --- |   --- --- --- --- ---
            Nonlinear general       nlc - nlnc
            Nonlinear network       nlnc
            Linear network          lnc
            Linear general          n_con - (nlc + lnc)
        */

        /** Nonlinear general constraints. */
        vector<string> cnames_nl_general = {};

        /** Nonlinear network constraints. */
        vector<string> cnames_nl_network = {};

        /** Linear network constraints. */
        vector<string> cnames_lin_network = {};

        /** Linear general constraints. */
        vector<string> cnames_lin_general = {};

        // Count of algebraic constraints:
        // The header needs to know how many range algebraic constraints and equality algebraic constraints we have.
        /** Number of range algebraic constraints */
        int nb_alg_cons_range = 0;
        /** equality algebraic constraints */
        int nb_alg_cons_eq = 0;


        /* *** *** *** Counts *** *** *** */
        // When the phase 2 is done, all the following counts should be available.
        // taken from "hooking your solver" and used in the above explanatios

        // --- --- --- Jacobian count
        int _jacobian_count = 0;
        int jacobian_count() const;


        // --- --- --- Variable counts

        /** Total number of variables. */
        int n_var() const;

        /** Number of variables appearing nonlinearly in constraints. */
        int nlvc() const;

        /** Number of variables appearing nonlinearly in objectives. */
        int nlvo() const;

        /** Number of variables appearing nonlinearly in both constraints and objectives.*/
        int nlvb() const;

        /** Number of integer variables appearing nonlinearly in both constraints and objectives.*/        
        int nlvbi() const;

        /** Number of integer variables appearing nonlinearly in constraints **only**.*/        
        int nlvci() const;

        /** Number of integer variables appearing nonlinearly in objectives **only**.*/        
        int nlvoi() const;

        /** Number of linear arcs .*/
        int nwv() const;

        /** Number of "other" integer variables.*/
        int niv() const;

        /** Number of binary variables.*/        
        int nbv() const;




        /** *** *** *** Segments *** *** *** **/
        /*
        NLS_Bound           b_segment;
        NLS_Range           r_segment;
        NLS_OSeg            o_segment;
        vector<NLS_CSeg>    c_segments = {};                // Constraint segments
        vector<NLS_JSeg>    j_segments = {};                // Constraint segments: linear part
        vector<NLS_LSeg>    l_segments = {};                // Logical constraint segments
        */

        /* *** *** *** Other info *** *** *** */
        
        
        /* *** *** *** Constructor *** *** *** */
        
        NLFile() = default;

        NLFile(string problem_name):
            problem_name(problem_name){}

            /*
            b_segment(this), r_segment(this), o_segment(this) {}
            */

        
        /* *** *** *** Printable *** *** *** */

        /** Print the NLFile on a stream.
         *  Note: this is not the 'Printable' interface as we do not pass any nl_file (that would be 'this') as a reference.
         */
        ostream& print_on( ostream& o ) const;

    };

} // End of NameSpace MiniZinc

#endif