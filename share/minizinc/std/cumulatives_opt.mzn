include "fzn_cumulatives_opt.mzn";

predicate fzn_cumulatives_opt(
  array [int] of var opt int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  array [int] of var int: machine,
  array [int] of var int: bound,
  bool: is_upper_bound,
  int: min_machine,
);

% Shared implementation for all optional-task cumulatives overloads. Private: the
% public overloads below forward to it so that the two named-bound forms do not call
% (and therefore do not warn about) the deprecated positional form.
predicate cumulatives_helper(
  array [$$E] of var opt int: start,
  array [$$E] of var int: duration,
  array [$$E] of var int: resource,
  array [$$E] of var $$M: machine,
  array [$$M] of var int: bound,
  bool: is_upper_bound,
) =
  assert(
    index_sets_agree(start, duration) /\ index_sets_agree(start, resource) /\
      index_sets_agree(start, machine),
    "cumulatives: the 4 first array arguments must have identical index sets",
    if length(start) >= 1 then
      assert(lb_array(duration) >= 0, "cumulatives: duration must be non-negative") /\
        assert(
          lb_array(machine) >= min(index_set(bound)) /\
            ub_array(machine) <= max(index_set(bound)),
          "cumulatives: machine must be in the index set of the 5th argument",
        ) /\
        fzn_cumulatives_opt(
          index2int(start),
          index2int(duration),
          index2int(resource),
          index2int(enum2int(machine)),
          index2int(bound),
          is_upper_bound,
          enum2int(min(index_set(bound))),
        )
    endif,
  );

/** @group globals.scheduling
  We have a set of optional tasks given by start times \a start, duration
  \a duration, resource requirements \a resource, and machine \a machine. We also
  have a set of machine given by resource capacities \a upper_bound. The constraint
  requires that forall \p i, \a machine[\p i] is in the index set of \a upper_bound
  and that for each machine \p j and time instant \p t, either no task assigned to
  \p j executes at \p t, or the total resource requirement of such tasks is not greater
  than the given bound \a upper_bound[\p j].  Resource requirements can be positive
  (for consumption) or negative (for production).

  Assumptions:

  - forall \p i, \a duration[\p i] >= 0
*/
predicate cumulatives(
  array [$$E] of var opt int: start,
  array [$$E] of var int: duration,
  array [$$E] of var int: resource,
  array [$$E] of var $$M: machine,
  array [$$M] of var int: upper_bound,
) = cumulatives_helper(start, duration, resource, machine, upper_bound, true);

/** @group globals.scheduling
  We have a set of optional tasks given by start times \a start, duration
  \a duration, resource requirements \a resource, and machine \a machine. We also
  have a set of machine given by resource minima \a lower_bound. The constraint
  requires that forall \p i, \a machine[\p i] is in the index set of \a lower_bound
  and that for each machine \p j and time instant \p t, either no task assigned to
  \p j executes at \p t, or the total resource requirement of such tasks is not less
  than the given bound \a lower_bound[\p j].  Resource requirements can be positive
  (for consumption) or negative (for production).

  Assumptions:

  - forall \p i, \a duration[\p i] >= 0
*/
predicate cumulatives(
  array [$$E] of var opt int: start,
  array [$$E] of var int: duration,
  array [$$E] of var int: resource,
  array [$$E] of var $$M: machine,
  array [$$M] of var int: lower_bound,
) = cumulatives_helper(start, duration, resource, machine, lower_bound, false);

% Deprecated positional form kept for backwards compatibility; the Boolean flag has
% been replaced by the named upper_bound/lower_bound overloads above. Deprecation is
% declared in cumulatives_opt.mzn.deprecated.mzn.
predicate cumulatives(
  array [$$E] of var opt int: s,
  array [$$E] of var int: d,
  array [$$E] of var int: r,
  array [$$E] of var $$M: m,
  array [$$M] of var int: b,
  bool: upper,
) = cumulatives_helper(s, d, r, m, b, upper);
