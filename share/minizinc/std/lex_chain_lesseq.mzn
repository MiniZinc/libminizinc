include "fzn_lex_chain_lesseq_bool.mzn";
include "fzn_lex_chain_lesseq_bool_reif.mzn";
include "fzn_lex_chain_lesseq_int.mzn";
include "fzn_lex_chain_lesseq_int_reif.mzn";

predicate fzn_lex_chain_lesseq_bool(array [int, int] of var bool: xs);
predicate fzn_lex_chain_lesseq_int(array [int, int] of var int: xs);

/** @group globals.lexicographic
  Requires that the columns of matrix \a xs are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain_lesseq(array [$$E, $$F] of var bool: xs) =
  if card(index_set_2of2(xs)) > 1 then fzn_lex_chain_lesseq_bool(index2int(xs)) endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a xs are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain_lesseq(array [$$E, $$F] of var $$T: xs) =
  if card(index_set_2of2(xs)) > 1 then fzn_lex_chain_lesseq_int(index2int(enum2int(xs))) endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a xs are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain(array [$$E, $$F] of var bool: xs) = lex_chain_lesseq(xs);

/** @group globals.lexicographic
  Requires that the columns of matrix \a xs are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain(array [$$E, $$F] of var $$T: xs) = lex_chain_lesseq(xs);
