% Decomposition provided by Mats Carlsson
predicate fzn_cumulatives_decomp(
  array [int] of var int: starts,
  array [int] of var int: durations,
  array [int] of var int: resources,
  array [int] of var int: machines,
  array [int] of var int: bounds,
  bool: is_upper_bound,
) =
  let {
    set of int: Tasks = index_set(starts);
  } in forall (j in Tasks) (
    let {
      var bool: cov1 =
        exists (i in Tasks) (
          fzn_cumulatives_task_at(starts[i], durations[i], machines[i], starts[j], machines[j])
        );
      var bool: cov2 =
        exists (i in Tasks) (
          fzn_cumulatives_task_at(
            starts[i],
            durations[i],
            machines[i],
            starts[j] + durations[j],
            machines[j],
          )
        );
      var int: load1 =
        sum (i in Tasks) (
          resources[i] *
            fzn_cumulatives_task_at(starts[i], durations[i], machines[i], starts[j], machines[j])
        );
      var int: load2 =
        sum (i in Tasks) (
          resources[i] *
            fzn_cumulatives_task_at(
              starts[i],
              durations[i],
              machines[i],
              starts[j] + durations[j],
              machines[j],
            )
        );
    } in (is_upper_bound /\ cov1 -> load1 <= bounds[machines[j]]) /\
      (is_upper_bound /\ cov2 -> load2 <= bounds[machines[j]]) /\
      (not is_upper_bound /\ cov1 -> load1 >= bounds[machines[j]]) /\
      (not is_upper_bound /\ cov2 -> load2 >= bounds[machines[j]])
  );

predicate fzn_cumulatives_task_at(
  var int: si,
  var int: di,
  var int: mi,
  var int: at,
  var int: mach,
) = si <= at /\ si + di > at /\ mi = mach;
