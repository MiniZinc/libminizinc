#include <minizinc/solvers/nl/nl_file.hh>

/**
 *  A NL File reprensentation.
 *  The purpose of this file is mainly to be a writer.
 *  Most of the information come from 'nlwrite.pdf'
 *  Note:
 *      * a character '#' starts a comment until the end of line.
 *      * A new line is '\n'
 */

namespace MiniZinc {

    // ostream& operator<<( ostream& o, Printable& p ) {
    //     p.print_on(o);
    //     return o;
    // }


    // --- --- --- Header

    // --- Write the header
    // The header is composed of then lines.
    // We describe them belove as we proceed.
    // '#' starts a comment until the end of line.
    ostream& NLHeader::print_on(ostream& os) const {
        
        // 1st line:
        // 'g': file will be in text format
        // other numbers: as given in the doc (no other explanation... we can probably go with g0)
        os  << "g3 1 1 0" << " # problem " << problem_name << '\n';

        // 2nd line:
        os  << nb_vars << " "
            << nb_algebraic_constraints << " "
            << nb_objectives << " "
            << nb_range_constraints << " "
            << nb_equality_constraints << " "
            << nb_logical_constraints << " "
            << "# Nb of: variable, algebraic constraints (inc. range & equality constraints), objectives. "
            << "Nb of contraints: range, equality, logical.\n";

        // 3rd line: Nonlinear and complementary information
        os  << nb_nonlinear_constraints << " "
            << nb_nonlinear_objectives << " "
            << nb_complementarity_linear_conditions << " "
            << nb_complementarity_nonlinear_conditions << " "
            << nb_complementarity_items_double_inequalities << " "
            << nb_complemented_vars_non_zero_lowerbound << " "
            << "# Nb of nonlinear: constraints, objectives. "
            << "Nb of complementary: linear & nonlinear conditions, double inequalities, vars with non-0 lower bound.\n";

        // 4th line: Network constraints
        os  << nb_nonlinear_network_constraints << " "
            << nb_linear_network_constraints << " "
            << "# Nb of network constraints: nonlinear, linear.\n";

        // 5th line: nonlinear variables:
        os  << nb_nonlinear_vars_in_constraints << " "
            << nb_nonlinear_vars_in_objectives << " "
            << nb_nonlinear_vars_in_both << " "
            << "# Nb of non linear vars in: only constraints, only objectives, both.\n";

        // 6th line:
        os  << nb_linear_network_vars << " "
            << nb_functions << " "
            << "0 1 "
            << "# Nb of: linear network vars, functions. Floating point arithmetic mode (TEXT == 0). Flag: if 1, add .sol suffixe.\n";

        // 7th line: discrete variables
        os  << nb_linear_binary_vars << " "
            << nb_linear_integer_vars << " "
            << nb_nonlinear_integer_vars_in_both << " "
            << nb_nonlinear_integer_vars_in_constraints_only << " "
            << nb_nonlinear_integer_vars_in_objectives_only << " "
            << "# Nb of linear vars: binary, integer (non binary). "
            << "Nb of nonlinear integer vars in: constraints only, objectives only, both.\n";

        // 8th line: non zeros
        os  << nb_nonzero_jacobian << " "
            << nb_nonzero_objective_gradients << " "
            << "# Nb of non zeros in: jacobian, objective gradients.\n";

        // 9th line: name length
        os  << max_constraint_name_length << " "
            << max_vars_name_length << " "
            << "# Longest name among: contraints' name, vars' name.\n";

        // 10th line: common expressions
        os  << nb_common_exprs_in_both << " "
            << nb_common_exprs_in_constraints_only << " "
            << nb_common_exprs_in_objectives_only << " "
            << nb_common_exprs_in_single_constraint_only << " "
            << nb_common_exprs_in_single_objectives_only << " "
            << "# Nb of common expressions in: both, constraints only, objectives only, single constraint, single objective.\n";

        return os;
    }




    // --- --- --- Segments





    // --- --- --- NLFile

    ostream& NLFile::print_on(ostream& os) const{
        header.print_on(os);
        return os;
    }
    

    // --- --- --- --- --- --- VDECL

    void NLFile::add_vdecl_enum(){}

    void NLFile::add_vdecl_tystr(){}

    void NLFile::add_vdecl_array(string name, ASTExprVec<TypeInst> range, const Type& type, const Expression* domain){}

    void NLFile::add_vdecl(string name, const Type& type, const Expression* domain){
        // Ensure that the variable is not in the map, then insert it.
        assert(variable_index.find(name) != variable_index.end());
        int index = variable_index.size();
        variable_index[name]=index;
        // Check the type
    }

}
