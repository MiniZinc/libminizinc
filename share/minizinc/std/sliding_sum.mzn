include "fzn_sliding_sum.mzn";
include "fzn_sliding_sum_reif.mzn";

/** @group globals.math
  Requires that in each subsequence \a vs[\p i], ..., \a vs[\p i + \a seq - 1] the sum of the
  values belongs to the interval [\a low, \a up].
*/
predicate sliding_sum(int: low, int: up, int: seq, array [$$E] of var int: vs) =
  fzn_sliding_sum(low, up, seq, index2int(vs));
