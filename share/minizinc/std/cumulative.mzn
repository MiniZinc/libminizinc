include "all_different.mzn";
include "disjunctive.mzn";
include "fzn_cumulative.mzn";
include "fzn_cumulative_reif.mzn";

predicate fzn_cumulative(
  array [int] of var int: starts,
  array [int] of var int: durations,
  array [int] of var int: resources,
  var int: capacity,
);

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a starts, durations \a durations, and
  resource requirements \a resources, never require more than a global resource bound
  \a capacity at any one time.

  Assumptions:

  - forall \p i, \a durations[\p i] >= 0 and \a resources[\p i] >= 0
*/
predicate cumulative(
  array [$$E] of var int: starts,
  array [$$E] of var int: durations,
  array [$$E] of var int: resources,
  var int: capacity,
) =
  assert(
    index_set(starts) == index_set(durations) /\ index_set(starts) == index_set(resources),
    "cumulative: the 3 array arguments must have identical index sets",
  ) /\
    if length(starts) >= 1 then
      assert(
        lb_array(durations) >= 0 /\ lb_array(resources) >= 0,
        "cumulative: durations and resource usages must be non-negative",
      ) /\
        if let {
          int: mr = lb_array(resources);
          any: mri = arg_min(arrayXd(resources, [lb(r_i) | r_i in resources]));
        } in forall (i in index_set(resources)) (is_fixed(resources[i]) /\ (fix(resources[i]) + mr > ub(capacity) \/ i = mri)) then
          forall (i in index_set(resources)) (durations[i] = 0 \/ fix(resources[i]) <= capacity) /\
            if length(starts) = 1 then
              true
            elseif forall (i in index_set(durations)) (is_fixed(durations[i]) /\ fix(durations[i]) == 1) then
              all_different(starts)
            else
              disjunctive(starts, durations)
            endif
        else
          fzn_cumulative(index2int(starts), index2int(durations), index2int(resources), capacity)
        endif
    endif;
