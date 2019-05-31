include "alldifferent.mzn";
include "disjunctive.mzn";
include "fzn_cumulative.mzn";

/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s, durations \a d, and
  resource requirements \a r, never require more than a global resource bound
  \a b at any one time.

  Assumptions:
  - forall \p i, \a d[\p i] >= 0 and \a r[\p i] >= 0
*/
predicate cumulative(array[int] of var int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
    assert(index_set(s) == index_set(d) /\ index_set(s) == index_set(r),
        "cumulative: the 3 array arguments must have identical index sets",
    assert(lb_array(d) >= 0 /\ lb_array(r) >= 0,
        "cumulative: durations and resource usages must be non-negative",
    if is_fixed(b) /\
       forall(i in index_set(r))
             (is_fixed(r[i]) /\ fix(r[i]) + lb_array(r) > ub(b))
    then
         if forall(i in index_set(d))(is_fixed(d[i]) /\ fix(d[i]) == 1)
         then
            alldifferent(s)
         else
            disjunctive(s, d)
         endif
    else
       fzn_cumulative(s, d, r, b)
    endif
    ));
