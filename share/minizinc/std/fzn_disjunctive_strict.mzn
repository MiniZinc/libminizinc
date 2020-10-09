predicate fzn_disjunctive_strict(array[int] of var int: s,
                             array[int] of var int: d) =
        forall (i in index_set(d)) (d[i] >= 0) /\
        forall (i,j in index_set(d) where i<j) (
          s[i]+d[i] <= s[j] \/ s[j]+d[j] <= s[i]
        );
