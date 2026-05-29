include "fzn_lex_chain_less_bool.mzn";
include "fzn_lex_chain_less_bool_reif.mzn";
include "fzn_lex_chain_less_int.mzn";
include "fzn_lex_chain_less_int_reif.mzn";

/** @group globals.lexicographic
  Requires that the columns of matrix \a xs are lexicographically sorted,
  strictly increasing.
*/
predicate lex_chain_less(array [$$E, $$F] of var bool: xs) =
  if card(index_set_2of2(xs)) > 1 then fzn_lex_chain_less_bool(index2int(xs)) endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a xs are lexicographically sorted,
  strictly increasing.
*/
predicate lex_chain_less(array [$$E, $$F] of var $$T: xs) =
  if card(index_set_2of2(xs)) > 1 then fzn_lex_chain_less_int(index2int(enum2int(xs))) endif;
