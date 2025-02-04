include "all_different.mzn";
include "disjunctive.mzn";
include "fzn_cumulative.mzn";
include "fzn_cumulative_reif.mzn";

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s, durations \a d, and
  resource requirements \a r, never require more than a global resource bound
  \a b at any one time.

  Assumptions:

  - forall \p i, \a d[\p i] >= 0 and \a r[\p i] >= 0
*/
predicate cumulative(
  array [$$E] of var int: s,
  array [$$E] of var int: d,
  array [$$E] of var int: r,
  var int: b,
) =
  assert(
    index_set(s) == index_set(d) /\ index_set(s) == index_set(r),
    "cumulative: the 3 array arguments must have identical index sets",
  ) /\
    if length(s) >= 1 then
      assert(
        lb_array(d) >= 0 /\ lb_array(r) >= 0,
        "cumulative: durations and resource usages must be non-negative",
      ) /\
        if let {
          int: mr = lb_array(r);
          any: mri = arg_min(arrayXd(r, [lb(r_i) | r_i in r]));
        } in forall (i in index_set(r)) (is_fixed(r[i]) /\ (fix(r[i]) + mr > ub(b) \/ i = mri)) then
          forall (i in index_set(r)) (d[i] = 0 \/ fix(r[i]) <= b) /\
            if length(s) = 1 then
              true
            elseif forall (i in index_set(d)) (is_fixed(d[i]) /\ fix(d[i]) == 1) then
              all_different(s)
            else
              disjunctive(s, d)
            endif
        else
          fzn_cumulative(index2int(s), index2int(d), index2int(r), b)
        endif
    endif;
