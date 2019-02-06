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

    // Update LB
    void NLS_BoundItem::update_lb(double new_lb){
        switch(tag){
            // LB <= var <= UB. Same tag
            case NLS_BoundItem::LB_UB:{
                assert(new_lb<=ub);
                if(new_lb>lb){ lb = new_lb; }
                break;
            }
            // var <= UB. Update tag
            case NLS_BoundItem::UB:{
                assert(new_lb<=ub);
                tag=LB_UB;
                lb=new_lb;
                break;
            }
            // LB <= var. Same tag
            case NLS_BoundItem::LB:{
                if(new_lb>lb){ lb = new_lb; }
                break;
            }
            // No bound. Update tag
            case NLS_BoundItem::NONE:{
                tag = LB;
                lb=new_lb;
                break;
            }
            // LB = var = UB. Should not happen
            case NLS_BoundItem::EQ:{
                cerr << "Should not happen" << endl;
                assert(false);
            }
        }
    }

    // Update UB
    void NLS_BoundItem::update_ub(double new_ub){
        switch(tag){
            // LB <= var <= UB. Same tag
            case NLS_BoundItem::LB_UB:{
                assert(lb<=new_ub);
                if(new_ub<ub){ ub = new_ub; }
                break;
            }
            // var <= UB. Same tag
            case NLS_BoundItem::UB:{
                if(new_ub<ub){ ub = new_ub; }
                break;
            }
            // LB <= var. Update tag
            case NLS_BoundItem::LB:{
                assert(lb<=new_ub);
                tag=LB_UB;
                ub=new_ub;
                break;
            }
            // No bound. Update tag
            case NLS_BoundItem::NONE:{
                tag = UB;
                ub=new_ub;
                break;
            }
            // LB = var = UB. Should not happen
            case NLS_BoundItem::EQ:{
                cerr << "Should not happen" << endl;
                assert(false);
            }
        }
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