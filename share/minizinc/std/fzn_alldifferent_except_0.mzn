include "alldifferent_except.mzn";

predicate fzn_alldifferent_except_0(array[int] of var int: vs) = 
    alldifferent_except(vs, {0});

