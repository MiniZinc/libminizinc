include "disjunctive_strict_opt.mzn";
include "fzn_disjunctive_opt.mzn";
include "fzn_disjunctive_opt_reif.mzn";

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s and durations \a d
  do not overlap in time. Tasks with duration 0 can be scheduled at any time,
  even in the middle of other tasks. Start times are optional variables, so
  that absent tasks do not need to be scheduled.

  Assumptions:

  - forall \p i, \a d[\p i] >= 0
*/
predicate disjunctive(array[int] of var opt int: s,
                      array[int] of var int: d) =
    assert(index_set(s) == index_set(d),
        "disjunctive: the array arguments must have identical index sets",
        forall (i in index_set(d)) (d[i] >= 0) /\
        if (lb_array(d) > 0) then
          disjunctive_strict(s,d)
        else
          fzn_disjunctive_opt(s, d)
        endif
    );
