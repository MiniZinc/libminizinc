include "fzn_count_lt_float_reif.mzn";

predicate fzn_count_lt_float_par_reif(
  array [int] of var float: xs,
  float: value,
  int: n,
  var bool: b,
) = fzn_count_lt_float_reif(xs, value, n, b);

%-----------------------------------------------------------------------------%
