/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s and durations \a d
  do not overlap in time. Tasks with duration 0 CANNOT be scheduled at any time,
  but only when no other task is running. Start times are optional variables, so
  that absent tasks do not need to be scheduled.

  Assumptions:

  - forall \p i, \a d[\p i] >= 0
*/
predicate disjunctive_strict(array[int] of var opt int: s,
                             array[int] of var int: d) =
    assert(index_set(s) == index_set(d),
        "disjunctive: the array arguments must have identical index sets",
        forall (i in index_set(d)) (d[i] >= 0) /\
        forall (i,j in index_set(d) where i<j) (
          absent(s[i]) \/ absent(s[j]) \/
          deopt(s[i])+d[i] <= deopt(s[j]) \/
          deopt(s[j])+d[j] <= deopt(s[i])
        )
    );
