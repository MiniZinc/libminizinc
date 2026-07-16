include "disjunctive_strict.mzn";
include "fzn_disjunctive.mzn";
include "fzn_disjunctive_reif.mzn";

predicate fzn_disjunctive(array [int] of var int: start, array [int] of var int: duration);

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a start and duration \a duration
  do not overlap in time. Tasks with duration 0 can be scheduled at any time,
  even in the middle of other tasks.

  Assumptions:

  - forall \p i, \a duration[\p i] >= 0
*/
predicate disjunctive(array [$$T] of var int: start, array [$$T] of var int: duration) =
  assert(
    index_sets_agree(start, duration),
    "disjunctive: the array arguments must have identical index sets",
    if lb_array(duration) > 0 then
      disjunctive_strict(start, duration)
    else
      fzn_disjunctive(index2int(start), index2int(duration))
    endif,
  );
