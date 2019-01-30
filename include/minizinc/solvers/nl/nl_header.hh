#ifndef __MINIZINC_NL_HEADER_HH__
#define __MINIZINC_NL_HEADER_HH__

#include <string>
#include <minizinc/solvers/nl/nl_printable.hh>

using namespace std;

namespace MiniZinc {    
    
    // --- --- --- Header
    class NLHeader: public Printable{

        public:
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

        public:
        ostream& print_on( ostream& o ) const override ;

    }; //  End of Header
}

#endif