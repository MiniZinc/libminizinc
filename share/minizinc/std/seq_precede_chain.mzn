include "fzn_seq_precede_chain_int.mzn";
include "fzn_seq_precede_chain_int_reif.mzn";
include "fzn_seq_precede_chain_set.mzn";
include "fzn_seq_precede_chain_set_reif.mzn";

/** @group globals.lexicographic
  Requires that \a i precedes \a i+1 in the array \a xs for all positive \a i.
*/
predicate seq_precede_chain(array [int] of var int: xs) = fzn_seq_precede_chain_int(xs);

/** @group globals.lexicographic
  Requires that \a i appears in a set in array \a xs before \a i+1 for all positive \a i
*/
predicate seq_precede_chain(array [int] of var set of int: xs) = fzn_seq_precede_chain_set(xs);
