#include <minizinc/solvers/nl/nl_segments.hh>
#include <minizinc/solvers/nl/nl_file.hh>

namespace MiniZinc {

    // Private constructor.
    NLS_BoundItem::NLS_BoundItem(Bound tag, double lb, double ub, int index):
    tag(tag), lb(lb), ub(ub), index(index){}

    // Static public builders
    NLS_BoundItem NLS_BoundItem::make_bounded(double lb, double ub, int index){
        return NLS_BoundItem(LB_UB, lb, ub, index);
    }

    NLS_BoundItem NLS_BoundItem::make_ub_bounded(double ub, int index){
        return NLS_BoundItem(UB, 0, ub, index);
    }

    NLS_BoundItem NLS_BoundItem::make_lb_bounded(double lb, int index){
        return NLS_BoundItem(LB, lb, 0, index);
    }

    NLS_BoundItem NLS_BoundItem::make_nobound(int index){
        return NLS_BoundItem(NONE, 0, 0, index);
    }

    NLS_BoundItem NLS_BoundItem::make_equal(double val, int index){
        return NLS_BoundItem(EQ, val, val, index);
    }

    // Printer
    ostream& NLS_BoundItem::print_on(ostream& os) const {
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
    ostream& NLS_BoundSeg::print_on(ostream& os) const {
        os << "b # Bounds on variable (" << nl_file->header.nb_vars << ")" << endl;

        for (auto & name : nl_file->name_vars) {
            auto &v = nl_file->variables.at(name);
            os << v.bound << endl;
        }
        
        return os;
    }
}