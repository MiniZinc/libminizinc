include "fzn_count_neq.mzn";

predicate fzn_count_neq_par(array[int] of var int: x, int: y, int: c) =
  fzn_count_neq(x,y,c);

%-----------------------------------------------------------------------------%
