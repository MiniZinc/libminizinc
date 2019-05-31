include "disjunctive_strict.mzn";
include "fzn_disjunctive.mzn";

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s and durations \a d
  do not overlap in time. Tasks with duration 0 can be scheduled at any time,
  even in the middle of other tasks.

  Assumptions:
  - forall \p i, \a d[\p i] >= 0
*/
predicate disjunctive(array[int] of var int: s,
                      array[int] of var int: d) =
    assert(index_set(s) == index_set(d),
        "disjunctive: the array arguments must have identical index sets",
        if (lb_array(d) > 0) then
          disjunctive_strict(s,d)
        else
          fzn_disjunctive(s,d)
        endif
    );
