include "fzn_count_geq.mzn";

predicate fzn_count_geq_par(array[int] of var int: x, int: y, int: c) =
    fzn_count_geq(x,y,c);

%-----------------------------------------------------------------------------%
