predicate fzn_maximum_arg_int(array [int] of var int: xs, var int: idx) =
  % general case: min could be 0 or 1
  let {
    int: l = min(index_set(xs));
    int: u = max(index_set(xs));
    int: n = u - l + 1;
    array [int] of var int: w = array1d(l..u, [n * xs[j] + (u - j) | j in l..u]);
    var int: Mx = max(w);
  } in forall (j in l..u) ((idx != j) = (Mx > w[j]));

%%% only the new decomposition from argmax paper CP2020 submission 
