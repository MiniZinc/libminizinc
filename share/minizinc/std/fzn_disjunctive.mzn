/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s and durations \a d
  do not overlap in time. Tasks with duration 0 can be scheduled at any time,
  even in the middle of other tasks.

  Assumptions:
  - forall \p i, \a d[\p i] >= 0
*/
predicate fzn_disjunctive(array[int] of var int: s,
                      array[int] of var int: d) =
        forall (i in index_set(d)) (d[i] >= 0) /\
        forall (i,j in index_set(d) where i<j) (
            d[i]=0 \/ d[j]=0 \/ s[i]+d[i] <= s[j] \/ s[j]+d[j] <= s[i]
        );
