#include <minizinc/solvers/nl/nl_segments.hh>
#include <minizinc/solvers/nl/nl_file.hh>

namespace MiniZinc {

    // Print the objective 'O' segment
    ostream& NLS_OSeg::print_on(ostream& os) const {

        os  << "O0 " << minmax << " # Objectif (unique, so O0) and minimize (0) or maximize(1)." << endl;        
        
        for(auto &tok : expression_graph){
            os << tok << endl; 
        }

        return os;
    }
  
}