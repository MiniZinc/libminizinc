#ifndef __MINIZINC_NL_SEGMENTS_HH__
#define __MINIZINC_NL_SEGMENTS_HH__

#include <minizinc/solvers/nl/nl_printable.hh>

#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <utility>  // for pair

using namespace std;




namespace MiniZinc {

    class NLFile;
    

    /** An algebraic constraint */
    /*
    class AlgebraicCons {
        public:
        int             index;
        NLS_BoundItem   bound;

        public:
        AlgebraicCons() = default;
        AlgebraicCons(int index, NLS_BoundItem bound):
            index(index), bound(bound){}
    };
    */


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
        ostream& print_on( ostream& o, const NLFile& nl_file ) const override;
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