#include <minizinc/solvers/nl/nl_segments.hh>
#include <minizinc/solvers/nl/nl_file.hh>

namespace MiniZinc {

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

    // Add an item in the 'r' segment.
    // Update the NLFile in the background
    void NLS_Range::addConstraint(AlgebraicCons ac){
        (nl_file->algcons).push_back(ac);
    }

}