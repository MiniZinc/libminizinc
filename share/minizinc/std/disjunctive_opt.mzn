include "disjunctive_strict_opt.mzn";
include "fzn_disjunctive_opt.mzn";
include "fzn_disjunctive_opt_reif.mzn";

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a starts and durations \a durations
  do not overlap in time. Tasks with duration 0 can be scheduled at any time,
  even in the middle of other tasks. Start times are optional variables, so
  that absent tasks do not need to be scheduled.

  Assumptions:

  - forall \p i, \a durations[\p i] >= 0
*/
predicate disjunctive(array [$$T] of var opt int: starts, array [$$T] of var int: durations) =
  assert(
    index_sets_agree(starts, durations),
    "disjunctive: the array arguments must have identical index sets",
    forall (i in index_set(durations)) (durations[i] >= 0) /\
      if lb_array(durations) > 0 then
        disjunctive_strict(starts, durations)
      else
        fzn_disjunctive_opt(index2int(starts), index2int(durations))
      endif,
  );
