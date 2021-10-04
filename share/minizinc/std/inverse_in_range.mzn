include "fzn_inverse_in_range.mzn";
include "fzn_inverse_in_range_reif.mzn";

/** @group globals.channeling
  If the \a i th variable of the collection \a X is assigned to \a j and if \a j is in the index set of \a Y then the \a j th variable of the collection \a Y is assigned to \a i.

  Conversely, if the \a j th variable of the collection \a Y is assigned to \a i and if \a i is in the index set of \a X then the \a i th variable of the collection \a X is assigned to \a j.
*/
predicate inverse_in_range(
	array[$$A] of var $$B: X,
	array[$$B] of var $$A: Y,
) = fzn_inverse_in_range(X, Y);
