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

        /** Create a string representing the name (and unique identifier) from an identifier. */
        static string get_vname(const Id *id);

        /** Create a string representing the name (and unique identifier) of a variable from a variable declaration. */
        static string get_vname(const VarDecl &vd);

        /** Create a string representing the name (and unique identifier) of a constraint from a specific call expression. */
        static string get_cname(const Call& c);

        /** Extract an array literal from an expression. */
        static const ArrayLit& get_arraylit(const Expression* e);

        /** Create a vector of double from a vector containing Expression being integer literal IntLit. */
        static vector<double> from_vec_int(const ArrayLit& v_int);

        /** Create a vector of double from a vector containing Expression being float literal FloatLit. */
        static vector<double> from_vec_fp(const ArrayLit& v_fp);

        /** Create a vector of variable names from a vector containing Expression being identifier Id. */
        static vector<string> from_vec_id(const ArrayLit& v_id);



        /* *** *** *** Phase 1: collecting data from MZN *** *** *** */

        // Variables collection, identified by name
        // Needs ordering, see phase 2
        map<string, NLVar> variables={};

        // Algebraic constraints collection, identified by name
        // Needs ordering, see phase 2
        map<string, NLAlgCons> constraints={};

        // Logical constraints do not need ordering:
        vector<NLLogicalCons> logical_constraints={};


        // Objective field. Only one, so we do not need ordering.
        NLObjective objective = {};


        // Output arrays
        vector<NLArray> output_arrays = {};


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



        // --- --- --- Constraints analysis

        /** Add a constraint to the NL File.
         * This method is a dispatcher for all the other constraints methods below. */
        void analyse_constraint(const Call& c);

        

        // --- --- --- Helpers

        /** Create a token from an expression representing a variable.
         * ONLY USE FOR CONSTRAINT, NOT OBJECTIVES! (UPDATE VARIABLES FLAG FOR CONSTRAINTS)
         */
        NLToken get_tok_var(const Expression* e);

        /** Create a token from an expression representing either a variable or an integer numeric value.
         * ONLY USE FOR CONSTRAINT, NOT OBJECTIVES!
         */
        NLToken get_tok_var_int(const Expression* e);

        /** Create a token from an expression representing either a variable or a floating point numeric value.
         * ONLY USE FOR CONSTRAINT, NOT OBJECTIVES!
         */ 
        NLToken get_tok_var_fp(const Expression* e);

        /** Update an expression graph (only by appending token) with a linear combination
         *  of coefficients and variables.
         *  ONLY USE FOR CONSTRAINTS, NOT OBJECTIVES!
         */
        void make_SigmaMult(vector<NLToken>& expression_graph, const vector<double>& coeffs, const vector<string>& vars);



        // --- --- --- Linear Builders
        // Use an array of literals 'coeffs' := c.arg(0), an array of variables 'vars' := c.arg(1),
        // and a variable or literal 'value' := c.arg(2).
        // [coeffs] and value are fixed (no variable allowed).
        // The call is needed to create the name. However, the extraction of the coefficients and the value
        // is left to the calling function as this could be use with both integer and floating point
        // (we only have floating point in NL)

        /** Create a linear constraint [coeffs] *+ [vars] = value. */
        void lincons_eq(const Call& c, const vector<double>& coeffs, const vector<string>& vars, NLToken value);

        /** Create a linear constraint [coeffs] *+ [vars] <= value. */
        void lincons_le(const Call& c, const vector<double>& coeffs, const vector<string>& vars, NLToken value);

        /** Create a linear logical constraint [coeffs] *+ [vars] PREDICATE value.
         *  Use a generic comparison operator.
         *  Warnings:   - Creates a logical constraint
         *              - Only use for conmparisons that cannot be expressed with '=' xor '<='.
         */
        void lincons_predicate(const Call& c, NLToken::OpCode oc,
            const vector<double>& coeffs, const vector<string>& vars, NLToken value);



        // --- --- --- Non Linear Builders
        // For predicates, uses 2 variables or literals: x := c.arg(0), y := c.arg(1)
        // x PREDICATE y
        
        // For unary operations, uses 2 variables or literals: x := c.arg(0), y := c.arg(1)
        // OPEARTOR x = y

        // For binary operations, uses 3 variables or literals: x := c.arg(0), y := c.arg(1), and z := c.arg(2).
        // x OPERATOR y = z

        /** Create a non linear constraint x = y
         *  Use the jacobian and the bound on constraint to translate into x - y = 0
         *  Simply update the bound if one is a constant.
         */
        void nlcons_eq(const Call& c, NLToken x, NLToken y);

        /** Create a non linear constraint x <= y
         *  Use the jacobian and the bound on constraint to translate into x - y <= 0
         *  Simply update the bound if one is a constant.
         */
        void nlcons_le(const Call& c, NLToken x, NLToken y);

        /** Create a non linear constraint with a predicate: x PREDICATE y
         *  Use a generic comparison operator.
         *  Warnings:   - Creates a logical constraint
         *              - Only use for conmparisons that cannot be expressed with '=' xor '<='.
         */
        void nlcons_predicate(const Call& c, NLToken::OpCode oc, NLToken x, NLToken y);

        /** Create a non linear constraint with a binary operator: x OPERATOR y = z */
        void nlcons_operator_binary(const Call& c, NLToken::OpCode oc, NLToken x, NLToken y, NLToken z);

        /** Create a non linear constraint with a binary operator: x OPERATOR y = z.
         *  OPERATOR is now a Multiop, with a count of 2 (so the choice of the method to use depends on the LN implementation) */
        void nlcons_operator_binary(const Call& c, NLToken::MOpCode moc, NLToken x, NLToken y, NLToken z);

        /** Create a non linear constraint with an unary operator: OPERATOR x = y */
        void nlcons_operator_unary(const Call& c, NLToken::OpCode oc, NLToken x, NLToken y);

        /** Create a non linear constraint, specialized for log2 unary operator: Log2(x) = y */
        void nlcons_operator_unary_log2(const Call& c, NLToken x, NLToken y);



        // --- --- --- Integer Linear Constraints

        /** Linar constraint: [coeffs] *+ [vars] = value */
        void consint_lin_eq(const Call& c);

        /** Linar constraint: [coeffs] *+ [vars] =< value */
        void consint_lin_le(const Call& c);

        /** Linar constraint: [coeffs] *+ [vars] != value */
        void consint_lin_ne(const Call& c);



        // --- --- --- Integer Non Linear Predicate Constraints

        /** Non linear constraint x = y */
        void consint_eq(const Call& c);

        /** Non linear constraint x <= y */
        void consint_le(const Call& c);

        /** Non linear constraint x != y */
        void consint_ne(const Call& c);



        // --- --- --- Integer Non Linear Binary Operator Constraints

        /** Non linear constraint x + y = z */
        void consint_plus(const Call& c);

        /** Non linear constraint x * y = z */
        void consint_times(const Call& c);

        /** Non linear constraint x / y = z */
        void consint_div(const Call& c);

        /** Non linear constraint x mod y = z */
        void consint_mod(const Call& c);

        /** Non linear constraint x pow y = z */
        void int_pow(const Call& c);

        /** Non linear constraint max(x, y) = z */
        void int_max(const Call& c);

        /** Non linear constraint min(x, y) = z */
        void int_min(const Call& c);



        // --- --- --- Integer Non Linear Unary Operator Constraints

        void int_abs(const Call& c);



        // --- --- --- Floating Point Linear Constraints

        /** Linar constraint: [coeffs] *+ [vars] = value */
        void consfp_lin_eq(const Call& c);

        /** Linar constraint: [coeffs] *+ [vars] = value */
        void consfp_lin_le(const Call& c);

        /** Linar constraint: [coeffs] *+ [vars] != value */
        void consfp_lin_ne(const Call& c);

        /** Linar constraint: [coeffs] *+ [vars] < value */
        void consfp_lin_lt(const Call& c);



        // --- --- --- Floating Point Non Linear Predicate Constraints

        /** Non linear constraint x = y */
        void consfp_eq(const Call& c);

        /** Non linear constraint x <= y */
        void consfp_le(const Call& c);

        /** Non linear constraint x != y */
        void consfp_ne(const Call& c);

        /** Non linear constraint x < y */
        void consfp_lt(const Call& c);



        // --- --- --- Floating Point Non Linear Binary Operator Constraints

        /** Non linear constraint x + y = z */
        void consfp_plus(const Call& c);

        /** Non linear constraint x - y = z */
        void consfp_minus(const Call& c);

        /** Non linear constraint x * y = z */
        void consfp_times(const Call& c);

        /** Non linear constraint x / y = z */
        void consfp_div(const Call& c);

        /** Non linear constraint x mod y = z */
        void consfp_mod(const Call& c);

        /** Non linear constraint x pow y = z */
        void float_pow(const Call& c);

        /** Non linear constraint max(x, y) = z */
        void float_max(const Call& c);

        /** Non linear constraint min(x, y) = z */
        void float_min(const Call& c);



        // --- --- --- Floating Point Non Linear Unary Operator Constraints

        /** Non linear constraint abs x = y */
        void float_abs(const Call& c);

        /** Non linear constraint acos x = y */
        void float_acos(const Call& c);

        /** Non linear constraint acosh x = y */
        void float_acosh(const Call& c);

        /** Non linear constraint asin x = y */
        void float_asin(const Call& c);

        /** Non linear constraint asinh x = y */
        void float_asinh(const Call& c);

        /** Non linear constraint atan x = y */
        void float_atan(const Call& c);

        /** Non linear constraint atanh x = y */
        void float_atanh(const Call& c);

        /** Non linear constraint cos x = y */
        void float_cos(const Call& c);

        /** Non linear constraint cosh x = y */
        void float_cosh(const Call& c);

        /** Non linear constraint exp x = y */
        void float_exp(const Call& c);

        /** Non linear constraint ln x = y */
        void float_ln(const Call& c);

        /** Non linear constraint log10 x = y */
        void float_log10(const Call& c);

        /** Non linear constraint log2 x = y */
        void float_log2(const Call& c);

        /** Non linear constraint sqrt x = y */
        void float_sqrt(const Call& c);

        /** Non linear constraint sin x = y */
        void float_sin(const Call& c);

        /** Non linear constraint sinh x = y */
        void float_sinh(const Call& c);

        /** Non linear constraint tan x = y */
        void float_tan(const Call& c);

        /** Non linear constraint tanh x = y */
        void float_tanh(const Call& c);



        // --- --- --- Other

        /** Integer x to floating point y. Constraint x = y translated into x - y = 0. */
        void int2float(const Call& c);




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


            Order of variables (yes, the way things are counted is... "special".)
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

        /** Contained all ordered variable names. Mapping variable index -> variable name */
        vector<string>  vnames={};

        /** Mapping variable name -> variable index */
        map<string, int> variable_indexes={};



        // --- --- --- Simple tests

        bool has_integer_vars() const;

        bool has_continous_vars() const;



        // --- --- --- Variables  counts
        
        // When the phase 2 is done, all the following counts should be available.
        // taken from "hooking your solver" and used in the above explanatios

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

        /** Accumulation of Jacobian counts. */
        int jacobian_count() const;
        int _jacobian_count = 0;



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

        /** Contained all ordered algebraic (and network if they were implemented) constraints names.
         *  Mapping constraint index -> constraint name
         */
        vector<string>  cnames={};

        /** Mapping constraint name -> contraint index */
        map<string, int> constraint_indexes={};

        // Count of algebraic constraints:
        // The header needs to know how many range algebraic constraints and equality algebraic constraints we have.
        /** Number of range algebraic constraints */
        int nb_alg_cons_range = 0;
        /** equality algebraic constraints */
        int nb_alg_cons_eq = 0;


        
        /* *** *** *** Constructor *** *** *** */
        
        NLFile() = default;


        /* *** *** *** Printable *** *** *** */

        /** Print the NLFile on a stream.
         *  Note: this is not the 'Printable' interface as we do not pass any nl_file (that would be 'this') as a reference.
         */
        ostream& print_on( ostream& o ) const;

    };

} // End of NameSpace MiniZinc

#endif