include "fzn_sum_set.mzn";
include "fzn_sum_set_reif.mzn";

predicate fzn_sum_set(
  array [int] of int: values,
  array [int] of int: weights,
  var set of int: selected,
  var int: total,
);

/** @group globals.math
  Requires that the sum of the weights \a weights[\p i1]..\a weights[\p iN] equals \a total, 
  where \a values[\p i1]..\a values[\p iN] are the elements appearing in set \a selected
*/
predicate sum_set(array [$$X] of $$Y: values, array [$$X] of int: weights, var set of $$Y: selected, var int: total) =
  fzn_sum_set(index2int(enum2int(values)), index2int(weights), enum2int(selected), total);
