#ifndef __MINIZINC_NL_SEGMENTS_HH__
#define __MINIZINC_NL_SEGMENTS_HH__

#include <ostream>
#include <minizinc/solvers/nl/nl_printable.hh>
using namespace std;

/*  A NL File is composed of a header and several segments.
    Adding items in the nl file is done through adding segment (or adding item in a segment).
    As for the header, segment are printable.
    Segment are identified by a prefix, which should be one of (taken from table 13 in 'writing nl files'):
        F   imported function description
        S   suffix values
        V   defined variable definition (must precede V,C,L,O segments where used) (yes, I know, "V must preceed V"...)
        C   algebraic constraint body
        L   logical constraint expression
        O   objective function
        d   dual initial guess
        x   primal initial guess
        r   bounds on algebraic constraint bodies (“ranges”)
        b   bounds on variable
        k   Jacobian column counts (must precede all J segments)
        J   Jacobian sparsity, linear terms
        G   Gradient sparsity, linear terms
*/


namespace MiniZinc {

    class NLFile;

    /* Bound on variable: the 'b' segment.
     * Segment componsed of 'nbvar' lines following a line containing only a 'b'.
     * Hence, variables are represented positionnaly in that list (first line for the first variable, etc...).
     * Each line starts with a tag (integer 0=<tag=<4) followed by some floating point numbers.
     * Those numbers are bounds, and the tag tells us how to interpret them with respect to the variable.
     * In the following, 'V' represent the position of the variable
     * 
     * b            # Starting the segment  # Variable          # Tag in enum NLS_Bounditem::Bound
     * 0 1.1 3.4    # 1.1 =< V =< 3.4       First variable      LB_UB
     * 1 2.5        # V =< 2.5              Second variable     UB
     * 2 7          # 7 =< V                etc...              LB
     * 3            # no constraint                             NONE
     * 4 9.4        # V = 9.4                                   EQ
     *
     * This segment can only appears once. 
     */
     

    /* Represent a bound on one variable */
    class NLS_BoundItem: public Printable {
        public:

        // Tag type
        enum Bound {
            LB_UB,
            UB,
            LB,
            NONE,            
            EQ
        };

        public:
        Bound   tag=NONE;
        double  lb=0;
        double  ub=0;
        int     index=0;

        public:
        static NLS_BoundItem make_bounded(double lb, double ub, int index);
        static NLS_BoundItem make_ub_bounded(double ub, int index);
        static NLS_BoundItem make_lb_bounded(double lb, int index);
        static NLS_BoundItem make_nobound(int index);
        static NLS_BoundItem make_equal(double val, int index);

        void update_lb(double new_lb);
        void update_ub(double new_lb);

        private:
        NLS_BoundItem(Bound tag, double lb, double ub, int index);

        public:
        NLS_BoundItem() = default;
        ostream& print_on( ostream& o ) const override;
    };
    
    /* Represent the b segment. */
    class NLS_BoundSeg: public Printable {
        public:
        NLFile const* nl_file;

        public:
        NLS_BoundSeg(NLFile const* nl_file):nl_file(nl_file){}
        ostream& print_on( ostream& o ) const override;
    };
}

#endif