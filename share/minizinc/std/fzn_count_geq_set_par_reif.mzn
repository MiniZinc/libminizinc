include "fzn_count_geq_set_reif.mzn";

predicate fzn_count_geq_set_par_reif(
  array [int] of var set of int: x,
  set of int: y,
  int: c,
  var bool: b,
) = fzn_count_geq_set_reif(x, y, c, b);

%-----------------------------------------------------------------------------%
