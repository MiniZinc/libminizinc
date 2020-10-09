predicate fzn_alldifferent_except_0(array[int] of var int: vs) = 
    forall(i, j in index_set(vs) where i < j) (
       (vs[i] != 0 /\ vs[j] != 0) -> vs[i] != vs[j]
    );
