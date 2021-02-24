include "lex_chain_less_bool.mzn";
include "lex_chain_less_int.mzn";

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  strictly increasing.
*/
predicate lex_chain_less(array[int, int] of var bool: a) = 
    if 1>=card(index_set_2of2(a)) then true
    else lex_chain_less_bool(a) endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain_less(array[int, int] of var int: a) = 
    if 1>=card(index_set_2of2(a)) then true
    else lex_chain_less_int(a) endif;

