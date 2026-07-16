include "fzn_cumulative_opt.mzn";
include "fzn_cumulative_opt_reif.mzn";

predicate fzn_cumulative_opt(
  array [int] of var opt int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  var int: capacity,
);

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a start, duration \a duration, and
  resource requirements \a resource, never require more than a global resource bound
  \a capacity at any one time. Start times are optional variables, so
  that absent tasks do not need to be scheduled.

  Assumptions:

  - forall \p i, \a duration[\p i] >= 0 and \a resource[\p i] >= 0
*/
predicate cumulative(
  array [$$E] of var opt int: start,
  array [$$E] of var int: duration,
  array [$$E] of var int: resource,
  var int: capacity,
) =
  assert(
    index_set(start) == index_set(duration) /\ index_set(start) == index_set(resource),
    "cumulative: the 3 array arguments must have identical index sets",
    if length(start) >= 1 then
      assert(
        lb_array(duration) >= 0 /\ lb_array(resource) >= 0,
        "cumulative: duration and resource usages must be non-negative",
        fzn_cumulative_opt(index2int(start), index2int(duration), index2int(resource), capacity),
      )
    endif,
  );
