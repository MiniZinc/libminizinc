include "fzn_lex_chain_lesseq_bool.mzn";
include "fzn_lex_chain_lesseq_bool_reif.mzn";
include "fzn_lex_chain_lesseq_int.mzn";
include "fzn_lex_chain_lesseq_int_reif.mzn";

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain_lesseq(array[int, int] of var bool: a) = 
	if card(index_set_2of2(a)) > 1 then 
		fzn_lex_chain_lesseq_bool(a)
	endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain_lesseq(array[int, int] of var int: a) = 
	if card(index_set_2of2(a)) > 1 then
		fzn_lex_chain_lesseq_int(a)
	endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain(array[int, int] of var bool: a) = 
    lex_chain_lesseq(a);

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  non-decreasing.
*/
predicate lex_chain(array[int, int] of var int: a) = 
    lex_chain_lesseq(a);

