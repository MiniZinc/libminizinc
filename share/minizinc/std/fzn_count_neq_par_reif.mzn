include "fzn_count_neq_reif.mzn";

predicate fzn_count_neq_par_reif(
  array [int] of var int: xs,
  int: value,
  int: n,
  var bool: b,
) = fzn_count_neq_reif(xs, value, n, b);

%-----------------------------------------------------------------------------%
