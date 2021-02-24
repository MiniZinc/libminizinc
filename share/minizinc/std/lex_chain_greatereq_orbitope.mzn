include "lex_chain_lesseq_orbitope.mzn";

/** @group globals.lexicographic
  Requires that the columns of binary matrix \a a are
  lexicographically sorted, non-increasing.
  Moreover, the second parameter has the following meaning:
  0: no further constraints, 1: set-partitioning orbitope,
  2: set-packing orbitope
*/
predicate lex_chain_greatereq_orbitope(
        array[int, int] of var int: a, int: kind) = 
    if 1>=card(index_set_2of2(a)) then true
    else lex_chain_lesseq_orbitope(
        array2d( index_set_1of2(a), index_set_2of2(a),
            [a[i, max(index_set_2of2(a)) - j + min(index_set_2of2(a))] |
                i in index_set_1of2(a), j in index_set_2of2(a)]
        ),
        kind) endif;

