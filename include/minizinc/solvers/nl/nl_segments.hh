#ifndef __MINIZINC_NL_SEGMENTS_HH__
#define __MINIZINC_NL_SEGMENTS_HH__

#include <minizinc/solvers/nl/nl_printable.hh>
#include <minizinc/solvers/nl/nl_expressions.hh>

#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <utility>  // for pair

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
    

    /** Represent a bound on one variable.
     * See 'b' and 'r' segments.
     * # Text       # Starting the segment  # Variable          # Tag in enum NLS_Bounditem::Bound
     * 0 1.1 3.4    # 1.1 =< V =< 3.4       First variable      LB_UB
     * 1 2.5        # V =< 2.5              Second variable     UB
     * 2 7          # 7 =< V                etc...              LB
     * 3            # no constraint                             NONE
     * 4 9.4        # V = 9.4                                   EQ
     */
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

     /** Variable Record
     *  The variable record contains the name, index and type (is_integer -> integer or floating point)
     *  of the a variable. It also contains the variable bound as describe into nl_segment.hh.
     *  Note that in flatzinc, the bound are eigher absent ot present on both side.
     *  In contrast, nl support partial bounds.
     *  Partial bounds in flatzinc are expressed through contraints: while analysing constraints, we reconstruct
     *  partial bound at the variable level.
     */
    class Var {
        public:
        // string const*   name;
        string          name;
        int             index;
        bool            is_integer;
        NLS_BoundItem   bound;
        int             jacobian_count;
        bool            to_report;

        public:
        Var() = default;
        Var(const string& name, int index, bool is_integer, NLS_BoundItem bound, bool to_report):
            name(name), index(index), is_integer(is_integer), bound(bound), jacobian_count(0), to_report(to_report){}

        Var copy_with_bound(NLS_BoundItem bound) const;
    };

    /** An algebraic constraint */
    class AlgebraicCons {
        public:
        int             index;
        NLS_BoundItem   bound;

        public:
        AlgebraicCons() = default;
        AlgebraicCons(int index, NLS_BoundItem bound):
            index(index), bound(bound){}
    };


    /** *** *** *** Segments *** *** *** **/
    
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
    class NLS_Bound: public Printable {
        public:
        NLFile* nl_file;

        public:
        size_t length();

        public:
        NLS_Bound(NLFile* nl_file):nl_file(nl_file){}
        ostream& print_on( ostream& o ) const override;
    };



    /** Bounds on algebraic constraint: the 'r' segment (For 'range'. Maybe.).
     * Works as the 'b' segment, but for algebraic constraints.
     * In principle, the 'r' segment can have 'complementary constraints' tagged by the integer '5'.
     * However, this should not be needed in our case, so we can reuse the NLS_BoundItem class.
     * 
     * This segment can only appears once. 
     */
    class NLS_Range: public Printable {
        public:
        NLFile* nl_file;

        public:
        void addConstraint(AlgebraicCons ac);

        public:
        NLS_Range(NLFile* nl_file):nl_file(nl_file){}
        ostream& print_on( ostream& o ) const override;
    };

    /** A Constraint 'C' Segment.
     * Can have several, so NLFile contains a vector of those.
     */
    class NLS_CSeg: public Printable {
        public:
        NLFile* nl_file;
        int constraint_idx;
        vector<NLToken> expression_graph = {};

        public:
        NLS_CSeg(NLFile* nl_file, int constraint_idx): nl_file(nl_file), constraint_idx(constraint_idx){}
        ostream& print_on( ostream& o ) const override;
    };



    /** A Constraint linear part 'J' Segment.
     * Can have several, so NLFile contains a vector of those.
     */
    class NLS_JSeg: public Printable {
        public:

        NLFile* nl_file;
        int constraint_idx;
        vector<pair<int, double>> var_coeff = {};

        public:
        NLS_JSeg(NLFile* nl_file, int constraint_idx): nl_file(nl_file), constraint_idx(constraint_idx){}
        ostream& print_on( ostream& o ) const override;
    };


    /** A Logical constrain 'L' Segment.
     * Can have several, so NLFile contains a vector of those.
     */
    class NLS_LSeg: public Printable {
        public:
        NLFile* nl_file;
        int constraint_idx;
        vector<NLToken> expression_graph = {};

        public:
        NLS_LSeg(NLFile* nl_file, int constraint_idx): nl_file(nl_file), constraint_idx(constraint_idx){}
        ostream& print_on( ostream& o ) const override;
    };


    /** An Objective segment 'O'.
     * In an NL file, we can have several of those.
     * However, in flatzinc, only one is allowed, so we only have one.
     */
     class NLS_OSeg: public Printable {
        public:
            enum MinMax{
                MINIMIZE = 0,
                MAXIMIZE = 1
            };

        NLFile* nl_file;
        MinMax minmax = MINIMIZE;
        vector<NLToken> expression_graph = {};

        public:
        NLS_OSeg(NLFile* nl_file):nl_file(nl_file){}
        ostream& print_on( ostream& o ) const override;
     };


}

#endif