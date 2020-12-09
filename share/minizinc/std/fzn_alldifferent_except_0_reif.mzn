include "alldifferent_except.mzn";

predicate fzn_alldifferent_except_0_reif(array[int] of var int: vs, var bool: b) = 
    alldifferent_except_reif(vs,{0},b);
