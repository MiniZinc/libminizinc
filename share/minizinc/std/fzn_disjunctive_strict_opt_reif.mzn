predicate fzn_disjunctive_strict_opt_reif(array[int] of var opt int: s,
                                          array[int] of var int: d,
                                          var bool: b) =
        b <-> ( forall (i in index_set(d)) (d[i] >= 0) /\
                forall (i,j in index_set(d) where i<j) (
                  absent(s[i]) \/ absent(s[j]) \/
                  deopt(s[i])+d[i] <= deopt(s[j]) \/
                  deopt(s[j])+d[j] <= deopt(s[i])
                )
              );
