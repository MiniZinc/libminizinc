/** @group globals.scheduling
  Requires that a set of tasks given by start times \a s, durations \a d, and
  resource requirements \a r, never require more than a global resource bound
  \a b at any one time.

  Assumptions:
  - forall \p i, \a d[\p i] >= 0 and \a r[\p i] >= 0

  Linear version.
*/

predicate fzn_cumulative(
  array [int] of var int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  var int: b :: promise_ctx_monotone,
) =
  if mzn_in_redundant_constraint() /\ fMZN__IgnoreRedundantCumulative then
    true
  else
    let {
      set of int: tasks = {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0};
      set of int: times = dom_array([s[i] | i in tasks]);
    } in if 0 == card(tasks) then
      /*true*/
      0 == card(index_set(s)) \/ b >= 0
    elseif MZN__Cumulative_Fixed_d_r /\ is_fixed(d) /\ is_fixed(r) /\ is_fixed(b) then
      fzn_cumulative_fixed_d_r(s, fix(d), fix(r), fix(b))
    elseif nMZN__UnarySizeMax_cumul >= card(times) * card(tasks) then
      cumulative_time_decomp(s, d, r, b, times)
    else
      cumulative_task_decomp(s, d, r, b)
    endif
  endif;

%% Can be called with a given set of times:
predicate cumulative_set_times(
  array [int] of var int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  var int: b,
  set of int: TIMES01,
) =
  assert(
    index_set(s) == index_set(d) /\ index_set(s) == index_set(r),
    "cumulative: the 3 array arguments must have identical index sets",
    assert(
      lb_array(d) >= 0 /\ lb_array(r) >= 0,
      "cumulative: durations and resource usages must be non-negative",
      let {
        set of int: tasks = {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0};
        set of int: times = dom_array([s[i] | i in tasks]) intersect TIMES01;
      } in if false then
        cumulative_time_decomp(s, d, r, b, times)
      else
        cumulative_task_decomp(s, d, r, b)
      endif,
    ),
  );

predicate cumulative_time_decomp(
  array [int] of var int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  var int: b :: promise_ctx_monotone,
  set of int: TIMES01,
) =
  let {
    set of int: tasks = {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0};
    set of int: times = {
      i |
        i in min([lb(s[i]) | i in tasks])..max([ub(s[i]) + ub(d[i]) | i in tasks])
          where i in TIMES01,
    };
  } in forall (t in times) (
    b >=
      sum (i in tasks) (
        if is_fixed(d[i]) then
          bool2int(s[i] in t - fix(d[i]) + 1..t)
        else
          bool2int(s[i] <= t /\ t < s[i] + d[i])
        endif *
          r[i]
      )
  );

predicate cumulative_task_decomp(
  array [int] of var int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  var int: b :: promise_ctx_monotone,
) =
  let {
    set of int: tasks = {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0};
  } in forall (j in tasks) (
    b - r[j] >=
      sum (
        i in tasks where i != j /\ lb(s[i]) <= ub(s[j]) /\ lb(s[j]) < ub(s[i] + d[i]), %% -- seems slower on mspsp ???
      ) (
        r[i] * %% r[i] *  !
          bool2int(s[i] <= s[j] /\ s[j] < s[i] + d[i])
      )
  );

%% A global cumulative with SCIP: fixed d and r
predicate fzn_cumulative_fixed_d_r(
  array [int] of var int: s,
  array [int] of int: d,
  array [int] of int: r,
  int: b,
);
