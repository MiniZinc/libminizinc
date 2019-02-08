#include <minizinc/solvers/nl/nl_segments.hh>
#include <minizinc/solvers/nl/nl_file.hh>

namespace MiniZinc {

    // Print a 'J' segment
    ostream& NLS_JSeg::print_on(ostream& os) const {
        os  << "J" << constraint_idx << " " << " " << var_coeff.size() << " # Linear part of the contraint " << constraint_idx << endl;
            
        for (auto & v_c : var_coeff) {
            os << v_c.first << " " << v_c.second << " # " << nl_file->name_vars[v_c.first] << endl;
        }
        
        return os;
    }
  
}