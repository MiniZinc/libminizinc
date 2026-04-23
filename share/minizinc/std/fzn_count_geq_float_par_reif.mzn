include "fzn_count_geq_float_reif.mzn";

predicate fzn_count_geq_float_par_reif(
  array [int] of var float: x,
  float: y,
  int: c,
  var bool: b,
) = fzn_count_geq_float_reif(x, y, c, b);

%-----------------------------------------------------------------------------%
