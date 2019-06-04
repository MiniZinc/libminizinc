/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s, durations \a d, and
  resource requirements \a r, never require more than a global resource bound
  \a b at any one time. Start times are optional variables, so
  that absent tasks do not need to be scheduled.

  Assumptions:
  - forall \p i, \a d[\p i] >= 0 and \a r[\p i] >= 0
*/
predicate fzn_cumulative_opt(array[int] of var opt int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
            let { 
               set of int: tasks = 
                  {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0 },
               set of int: times =
                  min([ lb(s[i]) | i in tasks ]) ..
                  max([ ub(s[i]) + ub(d[i]) | i in tasks ]) 
                } 
            in
               forall( t in times ) (
                  b >= sum( i in tasks ) (
                     bool2int( occurs(s[i]) /\ deopt(s[i]) <= t /\ t < deopt(s[i]) + d[i] ) * r[i]
                  )
               );
