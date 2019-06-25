include "fzn_sum_pred.mzn";
include "fzn_sum_pred_reif.mzn";

/** @group globals
  Requires that the sum of \a cs[\p i1]..\a cs[\p iN] equals \a s,
  where \p i1..\p iN are the elements of the \a i th set in \a sets.
  
  Nb: not called 'sum' as in the constraints catalog because 'sum' is a
  MiniZinc built-in function.
*/
predicate sum_pred(var int: i, array[int] of set of int: sets,
                   array[int] of int: cs, var int: s) =
    fzn_sum_pred(i, sets, cs, s);
