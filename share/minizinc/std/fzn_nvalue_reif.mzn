predicate fzn_nvalue_reif(var int: n, array [int] of var int: x, var bool: b) =
  if x = [] then b <-> n=0 else
  let {
    int: lx = lb_array(x);
    int: ux = ub_array(x);
  } in b <-> n == sum (j in lx..ux) (exists (i in index_set(x)) (x[i] = j))
  endif;
