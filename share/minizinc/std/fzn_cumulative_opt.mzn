include "fzn_cumulative_opt_decomp.mzn";

predicate fzn_cumulative_opt(array[int] of var opt int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
    fzn_cumulative_opt_decomp(s,d,r,b);
