/**
 * Describe the structure of a NL file.
 * Main author: Matthieu Herrmann, Monash University, Melbourne, Australia. 2019
 **/

#ifndef __MINIZINC_NL_FILE_HH__
#define __MINIZINC_NL_FILE_HH__


#include <string>
#include <ostream>
#include <map>

#include <minizinc/astvec.hh>
#include <minizinc/ast.hh>

using namespace std;

// This files declare data-structure describing the various components of a nl files.
// A nl files is composed of two main parts: a header and a list of segments.
// The header contains statistics and meta information about the model.
// The segments allow to describe the model, i.e. the variables, the constraints and objectives.
// The order of the segments and, when relevant, the order of their content
// (e.g. a segment declaring a list of variables) matters.

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




    // --- --- --- Header
    class NLHeader: public Printable{

        public:

        // --- --- --- --- --- --- --- --- --- Fields

        // --- --- --- 1st line: the name of the problem
        string problem_name;

        // --- --- --- 2nd line:
        /* Nb of variables */
        int nb_vars;
        /* Nb of algebreaic constraints.
            *  including nb_ranges_constraints and nb_equality_constraints
            *  excluding nb_logical_constraints */
        int nb_algebraic_constraints;
        /* Nb of objectives */
        int nb_objectives;
        /* Nb of range constraints -Inf < left < x < right < +Inf */
        int nb_range_constraints;
        /* Nb of equality constraints */
        int nb_equality_constraints;
        /* Nb of logical constraints */
        int nb_logical_constraints;

        // --- --- --- 3rd line:
        /* Nb of nonlinear constraints */
        int nb_nonlinear_constraints;
        /* Nb of nonlinear objectives */
        int nb_nonlinear_objectives;
        /* Nb of complementarity linear conditions */
        int nb_complementarity_linear_conditions;
        /* Nb of complementarity nonlinear conditions */
        int nb_complementarity_nonlinear_conditions;
        /* Nb of complementarity items involving double inequalities */
        int nb_complementarity_items_double_inequalities;
        /* Nb of complemented variables with non-zero lower bound */
        int nb_complemented_vars_non_zero_lowerbound;

        // --- --- --- 4th line: network constraints
        /* Nb of nonlinear network constraints */
        int nb_nonlinear_network_constraints;
        /* Nb of linear network constraints */
        int nb_linear_network_constraints;

        // --- --- --- 5th line: nonlinear variables:
        /* Nb of nonlinear variable in constraints
            * Warning: also account for the variable both in constraints and objectives! */
        int nb_nonlinear_vars_in_constraints;
        /* Nb of nonlinear variable in objectives
            * Warning: also account for the variable both in constraints and objectives! */
        int nb_nonlinear_vars_in_objectives;
        /* Nb of nonlinear variable in both constraints & objectives */
        int nb_nonlinear_vars_in_both;

        // --- --- --- 6th line:
        /* Nb of linear network variables */
        int nb_linear_network_vars;
        /* Nb of functions */
        int nb_functions;
        /* Floating point arithmetic mode. Matters for binary format, but we always use the text format. Put 0. */
        // int arith_kind;
        /* Flag: if 1, writes the .sol suffixes for outpout file, else, does not. We put it at 1.*/
        // int flags;

        // --- --- --- 7th line: discrete variables
        /* Nb of linear binary variables */
        int nb_linear_binary_vars;
        /* Nb of linear integer (excluding binary) variables */
        int nb_linear_integer_vars;
        /* Nb of non linear integer variables in both constraints and objectives */
        int nb_nonlinear_integer_vars_in_both;
        /* Nb of non linear integer variables ONLY in constraints
            * Warning: behviour differs from line 5! */
        int nb_nonlinear_integer_vars_in_constraints_only;
        /* Nb of non linear integer variables ONLY in constraints
            * Warning: behviour differs from line 5! */
        int nb_nonlinear_integer_vars_in_objectives_only;

        // --- --- --- 8th line: non zeros
        /* Nb of nonzeros in the Jacobian */
        int nb_nonzero_jacobian;
        /* Nb of nonzeros in the objective gradients */
        int nb_nonzero_objective_gradients;

        // --- --- --- 9th line: name length
        /* Lenght of the longest name among constraints' name */
        int max_constraint_name_length;
        /* Lenght of the longest name among variables' name */
        int max_vars_name_length;

        // --- --- --- 10th line: common expressions
        /* Nb of common expressions in both constraints and objectives */
        int nb_common_exprs_in_both;
        /* Nb of common expressions in constraints ONLY (excluding objectives) */
        int nb_common_exprs_in_constraints_only;
        /* Nb of common expressions in both objectives ONLY (excluding constraints) */
        int nb_common_exprs_in_objectives_only;
        /* Nb of common expressions appearing in a single constraint ONLY (excluding objectives) */
        int nb_common_exprs_in_single_constraint_only;
        /* Nb of common expressions appearing in a single objective ONLY (excluding constraints) */
        int nb_common_exprs_in_single_objectives_only;

        // --- --- --- --- --- --- --- --- --- Methods

        ostream& print_on( ostream& o ) const override ;

    }; //  End of Header


    // --- --- --- Segment
    // A NL File is composed of a header and several segments.
    // Adding items in the nl file is done through adding segment (or adding item in a segment).
    // In turn, segments are supposed to update the header accordingly, hence they need a reference to it.
    // As for the header, segment are printable.
    class NLSegment: public Printable {

        public:

        // --- --- --- --- --- --- --- --- --- Fields
        NLHeader& header;
        string pfx;

        /* pfx should be one of (taken from table 13 in 'writing nl files')
        F   imported function description
        S   suffix values
        V   defined variable definition (must precede V,C,L,O segments where used) (yes, I know, "V must preceed V"...)
        C   algebraic constraint body
        L   logical constraint expression
        O   objective function
        d   dual initial guess
        x   primal initial guess
        r   bounds on algebraic constraint bodies (“ranges”)
        b   bounds on variable
        k   Jacobian column counts (must precede all J segments)
        J   Jacobian sparsity, linear terms
        G   Gradient sparsity, linear terms
        */

        // --- --- --- --- --- --- --- --- --- Methods
        // --- --- --- Methods
        NLSegment(NLHeader& h, string pfx):header(h), pfx(pfx){}

    };

    // --- --- --- Segments specialisation
    
    // "b   bounds on variable"
    class NLSBounds: public NLSegment {
        public:
        // --- --- --- --- --- --- --- --- --- Fields
        
        // --- --- --- --- --- --- --- --- --- Methods
        
        NLSBounds(NLHeader& h):NLSegment(h, "b"){}
        
        ostream& print_on( ostream& o ) const override { return o; }
    };


    // --- --- --- Variable Declaration Reord
    class VDecl {
        public:
            // --- --- --- --- --- --- --- --- --- Fields
            bool is_integer;
            const string& name;
            int index;

            // --- --- --- Integer case
            long llb, lub;
            
            // --- --- --- Float case (!is_integer)
            double dlb, dub;

            // --- --- --- --- --- --- --- --- --- Methods
            VDecl(const string& n, int i, long lb, long ub):
                is_integer(true), name(n), index(i), llb(lb), lub(ub) { }

            VDecl(const string& n, int i, double lb, double ub):
                is_integer(false), name(n), index(i), dlb(lb), dub(ub) { }

    };


    // --- --- --- NL Files

    class NLFile: public Printable {

        public:

        // --- --- --- --- --- --- --- --- --- Fields
        NLHeader header={};                         // 0-init
        std::map<string,int> variable_index={};     // Mapping variable name -> variable index
        

        // --- --- --- --- --- --- --- --- --- Methods
        
        ostream& print_on( ostream& o ) const override ;

        // --- --- --- --- --- --- VDECL
        void add_vdecl_enum();
            
        void add_vdecl_tystr();

        void add_vdecl_array(const string& name, ASTExprVec<TypeInst> range, const Type& type, const Expression* domain);

        void add_vdecl(const string& name, const Type& type, const Expression* domain);

    };

} // End of NameSpace MiniZinc

#endif