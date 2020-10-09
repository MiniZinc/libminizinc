predicate fzn_alldifferent_except_0_reif(array[int] of var int: vs, var bool: b) = 
    b <-> forall(i, j in index_set(vs) where i < j) (
       (vs[i] != 0 /\ vs[j] != 0) -> vs[i] != vs[j]
    );
