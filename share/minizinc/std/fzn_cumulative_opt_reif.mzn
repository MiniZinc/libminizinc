predicate fzn_cumulative_opt_reif(array[int] of var opt int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b, var bool: bb) =
        let { 
            set of int: Tasks = 
                {i | i in index_set(s) where ub(occurs(s[i])) > 0 /\ ub(r[i]) > 0 /\ ub(d[i]) > 0 }
        } in
            if 0==card(Tasks) then bb <-> ( 0==card(index_set(s)) \/ b>=0 )
            else
                let {
                    int: early = min([ lb(s[i])            | i in Tasks ]), 
                    int: late  = max([ ub(s[i]) + ub(d[i]) | i in Tasks ]) 
                } in (
                    if late - early > 5000 then
                      fzn_cumulative_opt_task_reif(s, d, r, b, bb)
                    else
                      fzn_cumulative_opt_time_reif(s, d, r, b, bb)
                    endif
                )
            endif
            ;


predicate fzn_cumulative_opt_time_reif(array[int] of var opt int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b, var bool: bb) =
    let { 
        set of int: Tasks = 
            {i | i in index_set(s) where ub(occurs(s[i])) > 0 /\ ub(r[i]) > 0 /\ ub(d[i]) > 0 },
        int: early = min([ lb(s[i]) | i in Tasks ]), 
        int: late  = max([ ub(s[i]) + ub(d[i]) | i in Tasks ]) 
    } in (
        bb <-> forall( t in early..late ) (
            b >= sum( i in Tasks ) (
                (occurs(s[i]) /\ deopt(s[i]) <= t /\ t < deopt(s[i]) + d[i]) * r[i]
            )
        )
    );

predicate fzn_cumulative_opt_task_reif(array[int] of var opt int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b, var bool: bb) =
    let { 
        set of int: Tasks = 
            {i | i in index_set(s) where ub(occurs(s[i])) > 0 /\ ub(r[i]) > 0 /\ ub(d[i]) > 0 }
    } in (
        bb <-> forall( j in Tasks ) ( occurs(s[j]) -> 
            b >= r[j] + sum( i in Tasks where i != j ) ( 
                (occurs(s[i]) /\ deopt(s[i]) <= deopt(s[j]) /\ deopt(s[j]) < deopt(s[i]) + d[i] ) * r[i]
            )
        )
    );
