predicate fzn_nvalue_reif(var int: n, array [int] of var int: xs, var bool: b) =
  if length(xs) = 0 then b <-> n=0 else
  let {
    int: lx = lb_array(xs);
    int: ux = ub_array(xs);
  } in b <-> n == sum (j in lx..ux) (exists (i in index_set(xs)) (xs[i] = j))
  endif;
