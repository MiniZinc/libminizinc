% The constraint `set_of_int_array_union`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `set_of_int_array_union` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_set_of_int_array_union(
  list of var set of int: xs,
  var set of int: result,
) =
  if length(xs) = 0 then
    result = {}
  elseif length(xs) = 1 then
    result = xs[min(index_set(xs))]
  else
    let {
      int: l = min(index_set(xs));
      int: u = max(index_set(xs));
      array [l..u - 1] of var set of ub_array(xs): y;
    } in y[l] = xs[l] union xs[l + 1] /\
      forall (i in l + 2..u) (y[i - 1] = y[i - 2] union xs[i]) /\
      result = y[u - 1]
  endif;
