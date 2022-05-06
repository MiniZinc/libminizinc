include "fzn_sum_set.mzn";
include "fzn_sum_set_reif.mzn";

/** @group globals.math
  Requires that the sum of the weights \a ws[\p i1]..\a ws[\p iN] equals \a s, 
  where \a vs[\p i1]..\a vs[\p iN] are the elements appearing in set \a x
*/
predicate sum_set(
	array[$$X] of $$Y: vs,
	array[$$X] of int: ws,
	var set of $$Y: x,
	var int: s
) = fzn_sum_set(vs, ws, x, s);
