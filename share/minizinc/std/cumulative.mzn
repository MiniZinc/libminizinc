include "all_different.mzn";
include "disjunctive.mzn";
include "fzn_cumulative.mzn";
include "fzn_cumulative_reif.mzn";

predicate fzn_cumulative(
  array [int] of var int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  var int: capacity,
);

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a start, duration \a duration, and
  resource requirements \a resource, never require more than a global resource bound
  \a capacity at any one time.

  Assumptions:

  - forall \p i, \a duration[\p i] >= 0 and \a resource[\p i] >= 0
*/
predicate cumulative(
  array [$$E] of var int: start,
  array [$$E] of var int: duration,
  array [$$E] of var int: resource,
  var int: capacity,
) =
  assert(
    index_set(start) == index_set(duration) /\ index_set(start) == index_set(resource),
    "cumulative: the 3 array arguments must have identical index sets",
  ) /\
    if length(start) >= 1 then
      assert(
        lb_array(duration) >= 0 /\ lb_array(resource) >= 0,
        "cumulative: duration and resource usages must be non-negative",
      ) /\
        if let {
          int: mr = lb_array(resource);
          any: mri = arg_min(arrayXd(resource, [lb(r_i) | r_i in resource]));
        } in forall (i in index_set(resource)) (is_fixed(resource[i]) /\ (fix(resource[i]) + mr > ub(capacity) \/ i = mri)) then
          forall (i in index_set(resource)) (duration[i] = 0 \/ fix(resource[i]) <= capacity) /\
            if length(start) = 1 then
              true
            elseif forall (i in index_set(duration)) (is_fixed(duration[i]) /\ fix(duration[i]) == 1) then
              all_different(start)
            else
              disjunctive(start, duration)
            endif
        else
          fzn_cumulative(index2int(start), index2int(duration), index2int(resource), capacity)
        endif
    endif;
