include "fzn_lex_chain_less_bool.mzn";
include "fzn_lex_chain_less_bool_reif.mzn";
include "fzn_lex_chain_less_int.mzn";
include "fzn_lex_chain_less_int_reif.mzn";

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  strictly increasing.
*/
predicate lex_chain_less(array[int, int] of var bool: a) = 
	if card(index_set_2of2(a)) > 1 then
		fzn_lex_chain_less_bool(a)
	endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  strictly increasing.
*/
predicate lex_chain_less(array[int, int] of var int: a) = 
	if card(index_set_2of2(a)) > 1 then
		fzn_lex_chain_less_int(a)
	endif;

