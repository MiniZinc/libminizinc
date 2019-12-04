include "fzn_cumulative_opt.mzn";
include "fzn_cumulative_opt_reif.mzn";

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s, durations \a d, and
  resource requirements \a r, never require more than a global resource bound
  \a b at any one time. Start times are optional variables, so
  that absent tasks do not need to be scheduled.

  Assumptions:

  - forall \p i, \a d[\p i] >= 0 and \a r[\p i] >= 0
*/
predicate cumulative(array[int] of var opt int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
    assert(index_set(s) == index_set(d) /\ index_set(s) == index_set(r),
        "cumulative: the 3 array arguments must have identical index sets",
        assert(lb_array(d) >= 0 /\ lb_array(r) >= 0,
            "cumulative: durations and resource usages must be non-negative",
          fzn_cumulative_opt(s,d,r,b)
        )
    );
