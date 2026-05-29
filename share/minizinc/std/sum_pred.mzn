include "fzn_sum_pred.mzn";
include "fzn_sum_pred_reif.mzn";

/** @group globals.math
  Requires that the sum of \a coefficients[\p i1]..\a coefficients[\p iN] equals \a total,
  where \p i1..\p iN are the elements of the \a idx th set in \a sets.
  
  Nb: not called 'sum' as in the constraints catalog because 'sum' is a
  MiniZinc built-in function.
*/
predicate sum_pred(
  var $$X: idx,
  array [$$X] of set of $$Y: sets,
  array [$$Y] of int: coefficients,
  var int: total,
) = fzn_sum_pred(enum2int(idx), index2int(enum2int(sets)), index2int(coefficients), total);
