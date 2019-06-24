include "all_different.mzn";

predicate fzn_arg_sort_int_reif(array[int] of var int:x,
                                array[int] of var int:p,
                                var bool:b) =
   b <-> ( all_different(p) /\
           forall(j in 1..length(x)-1)
             (x[p[j]] <= x[p[j+1]] /\ (x[p[j]] == x[p[j+1]] -> p[j] < p[j+1]))
         );
