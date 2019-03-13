#include <minizinc/solvers/nl/nl_segments.hh>
#include <minizinc/solvers/nl/nl_file.hh>

namespace MiniZinc {

    /*** *** *** Segments *** *** ***/


    // --- --- --- B segment

    // Print the 'b' segment
    ostream& NLS_Bound::print_on(ostream& os, const NLFile &nl_file) const {
        os << "b # Bounds on variable (" << nl_file.variables.size() << ")" << endl;

        for (auto & name : nl_file.vnames) {
            auto &v = nl_file.variables.at(name);
            v.bound.print_on(os, name);
            os << endl;
            // os << v.bound << " # " << name << endl;
        }
        
        return os;
    }



    // --- --- --- C segment

    // Print a constraint 'C' segment
    ostream& NLS_CSeg::print_on(ostream& os) const {
        os  << "C" << constraint_idx << " # Constraint " << constraint_idx << endl;    

        for(auto &tok : expression_graph){
            os << tok << endl; 
        }

        return os;
    }



    // --- --- --- J segment

    // Print a 'J' segment
    ostream& NLS_JSeg::print_on(ostream& os) const {
        os  << "J" << constraint_idx << " " << " " << var_coeff.size()
            << " # Linear part of the constraint " << constraint_idx << endl;
            
        for (auto & v_c : var_coeff) {
            os << v_c.first << " " << v_c.second << " # " << nl_file->vnames[v_c.first] << endl;
        }
        
        return os;
    }


    // --- --- --- L segment

    // Print a logical constraint 'L' segment
    ostream& NLS_LSeg::print_on(ostream& os) const {
        os  << "L" << constraint_idx << " # Logical constraint " << constraint_idx << endl;    

        for(auto &tok : expression_graph){
            os << tok << endl; 
        }

        return os;
    }


    // --- --- --- O segment

    // Print the objective 'O' segment
    // Note: always one unique constraint in our case.
    ostream& NLS_OSeg::print_on(ostream& os) const {
        os  << "O0 " << minmax << " # Objectif (unique, so O0) and minimize (0) or maximize(1)." << endl;        
        
        for(auto &tok : expression_graph){
            os << tok << endl; 
        }

        return os;
    }


    // --- --- --- R segment

    // Add an item in the 'r' segment.
    // Update the NLFile in the background
    void NLS_Range::addConstraint(AlgebraicCons ac){
        (nl_file->algcons).push_back(ac);
    }

    // Print the 'r' segment
    ostream& NLS_Range::print_on(ostream& os) const {
        os  << "r # Bounds on algebraic constraint bodies ("
            << (nl_file->header.nb_algebraic_constraints)
            << ")" << endl;

        for (auto & cons : nl_file->algcons) {
            os << cons.bound << endl;
        }
        
        return os;
    }



}