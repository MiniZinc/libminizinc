include "fzn_count_lt_reif.mzn";

predicate fzn_count_lt_par_reif(array[int] of var int: x, int: y, int: c, var bool: b) =
  fzn_count_lt_reif(x,y,c,b);

%-----------------------------------------------------------------------------%
