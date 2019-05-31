include "fzn_seq_precede_chain_int.mzn";
include "fzn_seq_precede_chain_set.mzn";

/** @group globals.lexicographic
  Requires that \a i precedes \a i+1 in the array \a x for all positive \a i.
  
*/
predicate seq_precede_chain(array[int] of var int: x) =
    fzn_seq_precede_chain_int(x);

/** @group globals.lexicographic
  Requires that \a i appears in a set in array \a x before \a i+1 for all positive \a i

*/
predicate seq_precede_chain(array[int] of var set of int: x) =
    fzn_seq_precede_chain_set(x);
