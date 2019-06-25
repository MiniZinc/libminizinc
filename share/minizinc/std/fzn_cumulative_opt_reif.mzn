predicate fzn_cumulative_opt_reif(array[int] of var opt int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b, var bool: bb) =
            let { 
               set of int: tasks = 
                  {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0 },
               set of int: times =
                  min([ lb(s[i]) | i in tasks ]) ..
                  max([ ub(s[i]) + ub(d[i]) | i in tasks ]) 
                } 
            in
               bb <-> forall( t in times ) (
                  b >= sum( i in tasks ) (
                     bool2int( occurs(s[i]) /\ deopt(s[i]) <= t /\ t < deopt(s[i]) + d[i] ) * r[i]
                  )
               );
