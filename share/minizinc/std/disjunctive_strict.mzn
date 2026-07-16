include "fzn_disjunctive_strict.mzn";
include "fzn_disjunctive_strict_reif.mzn";

predicate fzn_disjunctive_strict(array [int] of var int: start, array [int] of var int: duration);

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a start and duration \a duration
  do not overlap in time. Tasks with duration 0 CANNOT be scheduled at any time,
  but only when no other task is running.

  Assumptions:

  - forall \p i, \a duration[\p i] >= 0
*/
predicate disjunctive_strict(array [$$T] of var int: start, array [$$T] of var int: duration) =
  assert(
    index_sets_agree(start, duration),
    "disjunctive: the array arguments must have identical index sets",
    fzn_disjunctive_strict(index2int(start), index2int(duration)),
  );
