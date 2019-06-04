/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s and durations \a d
  do not overlap in time. Tasks with duration 0 can be scheduled at any time,
  even in the middle of other tasks. Start times are optional variables, so
  that absent tasks do not need to be scheduled.

  Assumptions:
  - forall \p i, \a d[\p i] >= 0
*/
predicate fzn_disjunctive(array[int] of var opt int: s,
                      array[int] of var int: d) =
          forall (i,j in index_set(d) where i<j) (
            absent(s[i]) \/ absent(s[j]) \/
            d[i]=0 \/ d[j]=0 \/
            deopt(s[i])+d[i] <= deopt(s[j]) \/
            deopt(s[j])+d[j] <= deopt(s[i])
          );
