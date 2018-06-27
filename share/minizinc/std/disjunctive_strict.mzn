/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s and durations \a d
  do not overlap in time. Tasks with duration 0 CANNOT be scheduled at any time,
  but only when no other task is running.

  Assumptions:

  - forall \p i, \a d[\p i] >= 0
*/
predicate disjunctive_strict(array[int] of var int: s,
                             array[int] of var int: d) =
    assert(index_set(s) == index_set(d),
        "disjunctive: the array arguments must have identical index sets",
        forall (i in index_set(d)) (d[i] >= 0) /\
        forall (i,j in index_set(d) where i<j) (
          s[i]+d[i] <= s[j] \/ s[j]+d[j] <= s[i]
        )
    );
