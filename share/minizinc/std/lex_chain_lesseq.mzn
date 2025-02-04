include "fzn_lex_chain_lesseq_bool.mzn";
include "fzn_lex_chain_lesseq_bool_reif.mzn";
include "fzn_lex_chain_lesseq_int.mzn";
include "fzn_lex_chain_lesseq_int_reif.mzn";

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain_lesseq(array [$$E, $$F] of var bool: a) =
  if card(index_set_2of2(a)) > 1 then fzn_lex_chain_lesseq_bool(index2int(a)) endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain_lesseq(array [$$E, $$F] of var $$T: a) =
  if card(index_set_2of2(a)) > 1 then fzn_lex_chain_lesseq_int(index2int(enum2int(a))) endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain(array [$$E, $$F] of var bool: a) = lex_chain_lesseq(a);

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain(array [$$E, $$F] of var $$T: a) = lex_chain_lesseq(a);
