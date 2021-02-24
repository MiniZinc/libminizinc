include "lex_chain_lesseq.mzn";

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-increasing.
*/
predicate lex_chain_greatereq(array[int, int] of var bool: a) = 
    if 1>=card(index_set_2of2(a)) then true
    else lex_chain_lesseq(
        array2d( index_set_1of2(a), index_set_2of2(a),
            [a[i, max(index_set_2of2(a)) - j + min(index_set_2of2(a))] |
                i in index_set_1of2(a), j in index_set_2of2(a)]
        )
    ) endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-increasing.
*/
predicate lex_chain_greatereq(array[int, int] of var int: a) = 
    if 1>=card(index_set_2of2(a)) then true
    else lex_chain_lesseq(
        array2d( index_set_1of2(a), index_set_2of2(a),
            [a[i, max(index_set_2of2(a)) - j + min(index_set_2of2(a))] |
                i in index_set_1of2(a), j in index_set_2of2(a)]
        )
    ) endif;

