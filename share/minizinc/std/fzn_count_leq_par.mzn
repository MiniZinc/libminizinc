include "fzn_count_leq.mzn";

predicate fzn_count_leq_par(array[int] of var int: x, int: y, int: c) =
  fzn_count_leq(x,y,c);

%-----------------------------------------------------------------------------%
