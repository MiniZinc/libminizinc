include "global_cardinality_low_up.mzn";
predicate fzn_alldifferent_except(array[int] of var int: vs, set of int: S) = 
    %% if the variables in vs are bounded then use the gcc decomposition
    if forall(i in index_set(vs))(has_bounds(vs[i])) then
       let { set of int: A = dom_array(vs) diff S; } in 
       global_cardinality_low_up(vs, A, [0 | i in A], [1 | i in A])
    else
    %% otherwise use the neq decomposition
       forall(i, j in index_set(vs) where i < j)
             ((vs[i] in S /\ vs[j] in S) \/ vs[i] != vs[j])
    endif;
