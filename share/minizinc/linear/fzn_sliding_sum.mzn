predicate fzn_sliding_sum(int: low, int: up, int: window_size, array [int] of var int: xs) =
  let {
    int: lx = min(index_set(xs));
    int: ux = max(index_set(xs));
  } in forall (i in lx..ux - window_size + 1) (
    let {
      var int: sum_of_l = sum (j in i..i + window_size - 1) (xs[j]);
    } in low <= sum_of_l /\ sum_of_l <= up
  );
