include "fzn_cumulative_opt.mzn";
include "fzn_cumulative_opt_reif.mzn";

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a starts, durations \a durations, and
  resource requirements \a resources, never require more than a global resource bound
  \a capacity at any one time. Start times are optional variables, so
  that absent tasks do not need to be scheduled.

  Assumptions:

  - forall \p i, \a durations[\p i] >= 0 and \a resources[\p i] >= 0
*/
predicate cumulative(
  array [$$E] of var opt int: starts,
  array [$$E] of var int: durations,
  array [$$E] of var int: resources,
  var int: capacity,
) =
  assert(
    index_set(starts) == index_set(durations) /\ index_set(starts) == index_set(resources),
    "cumulative: the 3 array arguments must have identical index sets",
    if length(starts) >= 1 then
      assert(
        lb_array(durations) >= 0 /\ lb_array(resources) >= 0,
        "cumulative: durations and resource usages must be non-negative",
        fzn_cumulative_opt(index2int(starts), index2int(durations), index2int(resources), capacity),
      )
    endif,
  );
