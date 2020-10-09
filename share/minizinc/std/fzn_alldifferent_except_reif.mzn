predicate fzn_alldifferent_except_reif(array[int] of var int: vs,set of int: S, var bool: b) = 
    b <-> forall(i, j in index_set(vs) where i < j) (
       (vs[i] in S /\ vs[j] in S) \/ vs[i] != vs[j]
    );
