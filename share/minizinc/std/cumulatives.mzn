include "fzn_cumulatives.mzn";

predicate fzn_cumulatives(
  array [int] of var int: starts,
  array [int] of var int: durations,
  array [int] of var int: resources,
  array [int] of var int: machines,
  array [int] of var int: bounds,
  bool: is_upper_bound,
  int: min_machine,
);

% Shared implementation for all cumulatives overloads. Private: the public
% overloads below forward to it so that the two named-bound forms do not call
% (and therefore do not warn about) the deprecated positional form.
predicate cumulatives_helper(
  array [$$E] of var int: starts,
  array [$$E] of var int: durations,
  array [$$E] of var int: resources,
  array [$$E] of var $$M: machines,
  array [$$M] of var int: bounds,
  bool: is_upper_bound,
) =
  assert(
    index_sets_agree(starts, durations) /\ index_sets_agree(starts, resources) /\
      index_sets_agree(starts, machines),
    "cumulatives: the 4 first array arguments must have identical index sets",
    if length(starts) >= 1 then
      assert(lb_array(durations) >= 0, "cumulatives: durations must be non-negative") /\
        assert(
          lb_array(machines) >= min(index_set(bounds)) /\
            ub_array(machines) <= max(index_set(bounds)),
          "cumulatives: machines must be in the index set of the 5th argument",
        ) /\
        fzn_cumulatives(
          index2int(starts),
          index2int(durations),
          index2int(resources),
          index2int(enum2int(machines)),
          index2int(bounds),
          is_upper_bound,
          enum2int(min(index_set(bounds))),
        )
    endif,
  );

/** @group globals.scheduling
  We have a set of tasks given by start times \a starts, durations \a durations,
  resource requirements \a resources, and machines \a machines. We also have a set of
  machines given by resource capacities \a upper_bounds. The constraint requires that
  forall \p i, \a machines[\p i] is in the index set of \a upper_bounds and that for
  each machine \p j and time instant \p t, either no task assigned to \p j executes at
  \p t, or the total resource requirement of such tasks is not greater than the given
  bound \a upper_bounds[\p j].  Resource requirements can be positive (for consumption)
  or negative (for production).

  Assumptions:

  - forall \p i, \a durations[\p i] >= 0
*/
predicate cumulatives(
  array [$$E] of var int: starts,
  array [$$E] of var int: durations,
  array [$$E] of var int: resources,
  array [$$E] of var $$M: machines,
  array [$$M] of var int: upper_bounds,
) = cumulatives_helper(starts, durations, resources, machines, upper_bounds, true);

/** @group globals.scheduling
  We have a set of tasks given by start times \a starts, durations \a durations,
  resource requirements \a resources, and machines \a machines. We also have a set of
  machines given by resource minima \a lower_bounds. The constraint requires that
  forall \p i, \a machines[\p i] is in the index set of \a lower_bounds and that for
  each machine \p j and time instant \p t, either no task assigned to \p j executes at
  \p t, or the total resource requirement of such tasks is not less than the given
  bound \a lower_bounds[\p j].  Resource requirements can be positive (for consumption)
  or negative (for production).

  Assumptions:

  - forall \p i, \a durations[\p i] >= 0
*/
predicate cumulatives(
  array [$$E] of var int: starts,
  array [$$E] of var int: durations,
  array [$$E] of var int: resources,
  array [$$E] of var $$M: machines,
  array [$$M] of var int: lower_bounds,
) = cumulatives_helper(starts, durations, resources, machines, lower_bounds, false);

% Deprecated positional form kept for backwards compatibility; the Boolean flag has
% been replaced by the named upper_bounds/lower_bounds overloads above. Deprecation is
% declared in cumulatives.mzn.deprecated.mzn.
predicate cumulatives(
  array [$$E] of var int: s,
  array [$$E] of var int: d,
  array [$$E] of var int: r,
  array [$$E] of var $$M: m,
  array [$$M] of var int: b,
  bool: upper,
) = cumulatives_helper(s, d, r, m, b, upper);
