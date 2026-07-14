include "fzn_sliding_sum.mzn";
include "fzn_sliding_sum_reif.mzn";

predicate fzn_sliding_sum(int: low, int: up, int: window_size, array [int] of var int: xs);

/** @group globals.math
  Requires that in each subsequence \a xs[\p i], ..., \a xs[\p i + \a window_size - 1] the sum of the
  values belongs to the interval [\a low, \a up].
*/
predicate sliding_sum(int: low, int: up, int: window_size, array [$$E] of var int: xs) =
  fzn_sliding_sum(low, up, window_size, index2int(xs));
