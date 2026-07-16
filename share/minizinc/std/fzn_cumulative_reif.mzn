predicate fzn_cumulative_reif(
  array [int] of var int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  var int: capacity,
  var bool: bb,
) =
  let {
    set of int: Tasks = {
      i | i in index_set(start) where ub(resource[i]) > 0 /\ ub(duration[i]) > 0
    };
  } in if 0 == card(Tasks) then
    bb <-> (0 == card(index_set(start)) \/ capacity >= 0)
  else
    let {
      int: early = min([lb(start[i]) | i in Tasks]);
      int: late = max([ub(start[i]) + ub(duration[i]) | i in Tasks]);
    } in (
      if late - early > 5000 then
        fzn_cumulative_task_reif(start, duration, resource, capacity, bb)
      else
        fzn_cumulative_time_reif(start, duration, resource, capacity, bb)
      endif
    )
  endif;

predicate fzn_cumulative_time_reif(
  array [int] of var int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  var int: b,
  var bool: bb,
) =
  let {
    set of int: Tasks = {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0};
    int: early = min([lb(s[i]) | i in Tasks]);
    int: late = max([ub(s[i]) + ub(d[i]) | i in Tasks]);
  } in (
    bb <-> forall (t in early..late) (b >= sum (i in Tasks) ((s[i] <= t /\ t < s[i] + d[i]) * r[i]))
  );

predicate fzn_cumulative_task_reif(
  array [int] of var int: s,
  array [int] of var int: d,
  array [int] of var int: r,
  var int: b,
  var bool: bb,
) =
  let {
    set of int: Tasks = {i | i in index_set(s) where ub(r[i]) > 0 /\ ub(d[i]) > 0};
  } in (
    bb <->
      forall (j in Tasks) (
        b >= r[j] + sum (i in Tasks where i != j) ((s[i] <= s[j] /\ s[j] < s[i] + d[i]) * r[i])
      )
  );
