include "fzn_cumulatives.mzn";

/** @group globals.scheduling
  We have a set of tasks given by start times \a s, durations \a d,
  resource requirements \a r, and machines \a m. We also have a set of
  machines given by resource bounds \a b.  Finally, we have a Boolean
  flag \a upper.  The constraint requires that forall \p i, \a m[\p i]
  is in the index set of \a b and that for each machine \p j and time
  instant \p t, either no task assigned to \p j executes at \p \t, or
  the total resource requirement of such tasks is not greater
  than, if \a upper is true, or not less than, if \a upper is false,
  the given bound \a b[\a j].  Resource requirements can be positive
  (for consumption) or negative (for production).

  Assumptions:

  - forall \p i, \a d[\p i] >= 0
*/
predicate cumulatives(
  array[$$E] of var int: s,
  array[$$E] of var int: d,
  array[$$E] of var int: r,
  array[$$E] of var $$M: m,
  array[$$M] of var int: b,
  bool: upper
) =
  assert(
    index_sets_agree(s, d) /\
    index_sets_agree(s, r) /\
    index_sets_agree(s, m),
    "cumulatives: the 4 first array arguments must have identical index sets",
    if length(s) >= 1 then
      assert(
        lb_array(d) >= 0,
        "cumulatives: durations must be non-negative"
      ) /\
      assert(
        lb_array(m) >= min(index_set(b)) /\ ub_array(m) <= max(index_set(b)),
        "cumulatives: machines must be in the index set of the 5th argument"
      ) /\
      fzn_cumulatives(s, d, r, m, b, upper, min(index_set(b)))
    endif
  );
