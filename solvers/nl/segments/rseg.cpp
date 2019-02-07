#include <minizinc/solvers/nl/nl_segments.hh>
#include <minizinc/solvers/nl/nl_file.hh>

namespace MiniZinc {

    // Private constructor.
    NLS_BoundCons::NLS_BoundCons(Bound tag, double lb, double ub, int index):
    tag(tag), lb(lb), ub(ub), index(index){}

    // Static public builders
    NLS_BoundCons NLS_BoundCons::make_bounded(double lb, double ub, int index){
        return NLS_BoundCons(LB_UB, lb, ub, index);
    }

    NLS_BoundCons NLS_BoundCons::make_ub_bounded(double ub, int index){
        return NLS_BoundCons(UB, 0, ub, index);
    }

    NLS_BoundCons NLS_BoundCons::make_lb_bounded(double lb, int index){
        return NLS_BoundCons(LB, lb, 0, index);
    }

    NLS_BoundCons NLS_BoundCons::make_nobound(int index){
        return NLS_BoundCons(NONE, 0, 0, index);
    }

    NLS_BoundCons NLS_BoundCons::make_equal(double val, int index){
        return NLS_BoundCons(EQ, val, val, index);
    }


    // Printer
    ostream& NLS_BoundCons::print_on(ostream& os) const {
        switch(tag){
            case LB_UB:{
                os << "0 " << lb << " " << ub;
                break;
            }
            case UB:{
                os << "1 " << ub;
                break;
            }
            case LB:{
                os << "2 " << lb;
                break;
            }
            case NONE:{
                os << "3";
                break;
            }
            case EQ:{
                os << "4 " << lb;
                break;
            }
        }
        return os;
    }

    // Print the 'b' segment
    ostream& NLS_RangeSeg::print_on(ostream& os) const {
        os  << "r # Bounds on algebraic constraint bodies ("
            << (nl_file->header.nb_range_constraints + nl_file->header.nb_equality_constraints)
            << ")" << endl;
        
        return os;
    }
}

  
}