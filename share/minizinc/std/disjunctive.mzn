include "disjunctive_strict.mzn";
include "fzn_disjunctive.mzn";
include "fzn_disjunctive_reif.mzn";

predicate fzn_disjunctive(array [int] of var int: starts, array [int] of var int: durations);

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a starts and durations \a durations
  do not overlap in time. Tasks with duration 0 can be scheduled at any time,
  even in the middle of other tasks.

  Assumptions:

  - forall \p i, \a durations[\p i] >= 0
*/
predicate disjunctive(array [$$T] of var int: starts, array [$$T] of var int: durations) =
  assert(
    index_sets_agree(starts, durations),
    "disjunctive: the array arguments must have identical index sets",
    if lb_array(durations) > 0 then
      disjunctive_strict(starts, durations)
    else
      fzn_disjunctive(index2int(starts), index2int(durations))
    endif,
  );
