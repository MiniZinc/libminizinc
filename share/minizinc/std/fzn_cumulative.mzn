predicate fzn_cumulative(
  array [int] of var int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  var int: b,
) =
  let {
    set of int: Tasks = {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0};
  } in if 0 == card(Tasks) then
    /*true*/
    0 == card(index_set(s)) \/ b >= 0
  else
    let {
      int: early = min([lb(s[i]) | i in Tasks]);
      int: late = max([ub(s[i]) + ub(d[i]) | i in Tasks]);
    } in (
      if late - early > 5000 then
        fzn_cumulative_task(s, d, r, b)
      else
        fzn_cumulative_time(s, d, r, b)
      endif
    )
  endif;

predicate fzn_cumulative_time(
  array [int] of var int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  var int: b,
) =
  let {
    set of int: Tasks = {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0};
    int: early = min([lb(s[i]) | i in Tasks]);
    int: late = max([ub(s[i]) + ub(d[i]) | i in Tasks]);
  } in (forall (t in early..late) (b >= sum (i in Tasks) ((s[i] <= t /\ t < s[i] + d[i]) * r[i])));

predicate fzn_cumulative_task(
  array [int] of var int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  var int: b,
) =
  let {
    set of int: Tasks = {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0};
  } in (
    forall (j in Tasks) (
      % Note: i can equal j. If j has a duration of 0, then it is not considered.
      b >= sum (i in Tasks) ((s[i] <= s[j] /\ s[j] < s[i] + d[i]) * r[i])
    )
  );
