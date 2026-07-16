include "disjunctive_strict_opt.mzn";
include "fzn_disjunctive_opt.mzn";
include "fzn_disjunctive_opt_reif.mzn";

predicate fzn_disjunctive_opt(array [int] of var opt int: start, array [int] of var int: duration);

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a start and duration \a duration
  do not overlap in time. Tasks with duration 0 can be scheduled at any time,
  even in the middle of other tasks. Start times are optional variables, so
  that absent tasks do not need to be scheduled.

  Assumptions:

  - forall \p i, \a duration[\p i] >= 0
*/
predicate disjunctive(array [$$T] of var opt int: start, array [$$T] of var int: duration) =
  assert(
    index_sets_agree(start, duration),
    "disjunctive: the array arguments must have identical index sets",
    forall (i in index_set(duration)) (duration[i] >= 0) /\
      if lb_array(duration) > 0 then
        disjunctive_strict(start, duration)
      else
        fzn_disjunctive_opt(index2int(start), index2int(duration))
      endif,
  );
