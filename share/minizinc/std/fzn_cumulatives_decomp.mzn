% Decomposition provided by Mats Carlsson
predicate fzn_cumulatives_decomp(
  array [int] of var int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  array [int] of var int: machine,
  array [int] of var int: bound,
  bool: is_upper_bound,
) =
  let {
    set of int: Tasks = index_set(start);
  } in forall (j in Tasks) (
    let {
      var bool: cov1 =
        exists (i in Tasks) (
          fzn_cumulatives_task_at(start[i], duration[i], machine[i], start[j], machine[j])
        );
      var bool: cov2 =
        exists (i in Tasks) (
          fzn_cumulatives_task_at(
            start[i],
            duration[i],
            machine[i],
            start[j] + duration[j],
            machine[j],
          )
        );
      var int: load1 =
        sum (i in Tasks) (
          resource[i] *
            fzn_cumulatives_task_at(start[i], duration[i], machine[i], start[j], machine[j])
        );
      var int: load2 =
        sum (i in Tasks) (
          resource[i] *
            fzn_cumulatives_task_at(
              start[i],
              duration[i],
              machine[i],
              start[j] + duration[j],
              machine[j],
            )
        );
    } in (is_upper_bound /\ cov1 -> load1 <= bound[machine[j]]) /\
      (is_upper_bound /\ cov2 -> load2 <= bound[machine[j]]) /\
      (not is_upper_bound /\ cov1 -> load1 >= bound[machine[j]]) /\
      (not is_upper_bound /\ cov2 -> load2 >= bound[machine[j]])
  );

predicate fzn_cumulatives_task_at(
  var int: si,
  var int: di,
  var int: mi,
  var int: at,
  var int: mach,
) = si <= at /\ si + di > at /\ mi = mach;
