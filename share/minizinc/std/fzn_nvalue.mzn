predicate fzn_nvalue(
  var int: n,
  array [int] of var int: x,
) =
  if length(x) = 0 then n=0 else
  let {
    int: lx = lb_array(x);
    int: ux = ub_array(x);
  } in n == sum (j in lx..ux) (exists (i in index_set(x)) (x[i] = j))
  endif;
