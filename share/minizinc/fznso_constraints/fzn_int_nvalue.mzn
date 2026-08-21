% The constraint `int_nvalue`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_nvalue` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_int_nvalue(list of var int: xs, var int: n) =
  if length(xs) = 0 then n=0 else
  let {
    int: lx = lb_array(xs);
    int: ux = ub_array(xs);
  } in n == sum (j in lx..ux) (exists (i in index_set(xs)) (xs[i] = j))
  endif;
