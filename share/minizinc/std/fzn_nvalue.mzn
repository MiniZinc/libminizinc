predicate fzn_nvalue(
  var int: n,
  array [int] of var int: xs,
) =
  if length(xs) = 0 then n=0 else
  let {
    int: lx = lb_array(xs);
    int: ux = ub_array(xs);
  } in n == sum (j in lx..ux) (exists (i in index_set(xs)) (xs[i] = j))
  endif;
