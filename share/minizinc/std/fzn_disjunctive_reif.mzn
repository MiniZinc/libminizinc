predicate fzn_disjunctive_reif(array[int] of var int: s,
                               array[int] of var int: d,
                               var bool: b) =
        b <-> ( forall (i in index_set(d)) (d[i] >= 0) /\
                forall (i,j in index_set(d) where i<j) (
                    d[i]=0 \/ d[j]=0 \/ s[i]+d[i] <= s[j] \/ s[j]+d[j] <= s[i]
                )
              );
