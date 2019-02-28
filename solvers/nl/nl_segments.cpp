#include <minizinc/solvers/nl/nl_segments.hh>
#include <minizinc/solvers/nl/nl_file.hh>

namespace MiniZinc {

    /*** *** *** Bounds *** *** ***/

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
                os << "0 " << lb << " " << ub << " # " << lb << " =< body =< " << ub;
                break;
            }
            case UB:{
                os << "1 " << ub << " # body =< " << ub;
                break;
            }
            case LB:{
                os << "2 " << lb << " # " << lb << " =< body";
                break;
            }
            case NONE:{
                os << "3" << " # No constraint";
                break;
            }
            case EQ:{
                os << "4 " << lb << " # body = " << lb;
                break;
            }
        }
        return os;
    }


    /*** *** *** Var *** *** ***/

    Var Var::copy_with_bound(NLS_BoundItem bound) const {
        Var v = Var(*this); // copy construct
        v.bound = bound;
        return v; //return Var(this->name, this->index, this->is_integer, bound, this->to_report);
    }


    /*** *** *** Segments *** *** ***/


    // --- --- --- B segment

    size_t NLS_Bound::length(){return nl_file->variables.size();}

    // Print the 'b' segment
    ostream& NLS_Bound::print_on(ostream& os) const {
        os << "b # Bounds on variable (" << nl_file->header.nb_vars << ")" << endl;

        for (auto & name : nl_file->name_vars) {
            auto &v = nl_file->variables.at(name);
            os << v.bound << " # " << name << endl;
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
            os << v_c.first << " " << v_c.second << " # " << nl_file->name_vars[v_c.first] << endl;
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