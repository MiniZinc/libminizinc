include "alldifferent.mzn";
include "disjunctive.mzn";

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s, durations \a d, and
  resource requirements \a r, never require more than a global resource bound
  \a b at any one time.

  Assumptions:
  - forall \p i, \a d[\p i] >= 0 and \a r[\p i] >= 0
*/
predicate cumulative(array[int] of var int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
    assert(index_set(s) == index_set(d) /\ index_set(s) == index_set(r),
        "cumulative: the 3 array arguments must have identical index sets",
    assert(lb_array(d) >= 0 /\ lb_array(r) >= 0,
        "cumulative: durations and resource usages must be non-negative",
    if forall(i in index_set(r))(is_fixed(r[i]) /\ fix(r[i]) == 1) /\
       is_fixed(b) /\ fix(b) == 1 then
        if forall(i in index_set(d))(is_fixed(d[i]) /\ fix(d[i]) == 1) then
            alldifferent(s)
        else
            disjunctive(s, d)
        endif
    else
        let { 
            set of int: Tasks = 
                {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0 }
        } in
            if 0==card(Tasks) then /*true*/ 0==card(index_set(s)) \/ b>=0
            else
                let {
                    int: early = min([ lb(s[i])            | i in Tasks ]), 
                    int: late  = max([ ub(s[i]) + ub(d[i]) | i in Tasks ]) 
                } in (
                    if late - early > 5000 then
                      cumulative_task(s, d, r, b)
                    else
                      cumulative_time(s, d, r, b)
                    endif
                )
            endif
    endif
    ));


predicate cumulative_time(array[int] of var int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
    let { 
        set of int: Tasks = 
            {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0 },
        int: early = min([ lb(s[i]) | i in Tasks ]), 
        int: late  = max([ ub(s[i]) + ub(d[i]) | i in Tasks ]) 
    } in (
        forall( t in early..late ) (
            b >= sum( i in Tasks ) (
                bool2int(s[i] <= t /\ t < s[i] + d[i]) * r[i]
            )
        )
    );

predicate cumulative_task(array[int] of var int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
    let { 
        set of int: Tasks = 
            {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0 }
    } in (
        forall( j in Tasks ) ( 
            b >= r[j] + sum( i in Tasks where i != j ) ( 
                bool2int(s[i] <= s[j] /\ s[j] < s[i] + d[i] ) * r[i]
            )
        )
    );

