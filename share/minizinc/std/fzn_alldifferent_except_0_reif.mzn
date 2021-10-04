include "fzn_alldifferent_except_reif.mzn";

predicate fzn_alldifferent_except_0_reif(array[int] of var int: vs, var bool: b) = 
    fzn_alldifferent_except_reif(vs,{0},b);
