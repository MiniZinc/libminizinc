% Decomposition provided by Mats Carlsson
predicate fzn_cumulatives_decomp(array[int] of var int: s,
                          array[int] of var int: d,
                          array[int] of var int: r,
                          array[int] of var int: m,
                          array[int] of var int: b,
                          bool: upper) =
  let {
    set of int: Tasks = index_set(s);
  } in
    forall (j in Tasks) (
      let {
        var bool: cov1 = exists(i in Tasks)(fzn_cumulatives_task_at(s[i], d[i], m[i], s[j], m[j]));
        var bool: cov2 = exists(i in Tasks)(fzn_cumulatives_task_at(s[i], d[i], m[i], s[j] + d[j], m[j]));
        var int: load1 = sum(i in Tasks)(r[i] * fzn_cumulatives_task_at(s[i], d[i], m[i], s[j], m[j]));
        var int: load2 = sum(i in Tasks)(r[i] * fzn_cumulatives_task_at(s[i], d[i], m[i], s[j] + d[j], m[j]));
      } in
        (upper /\ cov1 -> load1 <= b[m[j]]) /\
        (upper /\ cov2 -> load2 <= b[m[j]]) /\
        (not upper /\ cov1 -> load1 >= b[m[j]]) /\
        (not upper /\ cov2 -> load2 >= b[m[j]])
    );

predicate fzn_cumulatives_task_at(var int: si,
                                  var int: di,
                                  var int: mi,
                                  var int: at,
                                  var int: mach) =
  si <= at /\ si + di > at /\ mi = mach;
