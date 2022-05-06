include "fzn_sum_pred.mzn";
include "fzn_sum_pred_reif.mzn";

/** @group globals.math
  Requires that the sum of \a cs[\p i1]..\a cs[\p iN] equals \a s,
  where \p i1..\p iN are the elements of the \a i th set in \a sets.
  
  Nb: not called 'sum' as in the constraints catalog because 'sum' is a
  MiniZinc built-in function.
*/
predicate sum_pred(
	var $$X: i,
	array[$$X] of set of $$Y: sets,
	array[$$Y] of int: cs,
	var int: s
) = fzn_sum_pred(i, sets, cs, s);
