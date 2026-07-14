predicate fzn_maximum_arg_float(array [int] of var float: xs, var int: idx) =
  let {
    int: l = min(index_set(xs));
    int: u = max(index_set(xs));
    float: uy = ub_array(xs);
  } in if exists (j in l..u) (lb(xs[j]) = uy) then
    let {
      array [l..u] of var bool: d;
    } in % max is known to be uy
    xs[idx] = uy /\ % ith case must be equal to ub
      forall (j in l..u) (xs[j] = uy -> idx <= j) /\ % lower bound
      d[l] = (xs[l] = uy) /\
      forall (j in l + 1..u) (d[j] <-> (d[j - 1] \/ (xs[j] = uy))) /\
      forall (j in l..u) (not d[j] -> idx >= j + 1) % upper bound
  else
    let {
      float: ly = lb_array(xs);
      array [l..u] of var ly..uy: y;
      array [l..u] of var l..u: mi;
    } in y[l] = xs[l] /\
      mi[l] = l /\
      idx = mi[u] /\
      forall (j in l + 1..u) (
        y[j] == max(xs[j], y[j - 1]) /\ mi[j] = if y[j - 1] >= xs[j] then mi[j - 1] else j endif
      )
  endif;
