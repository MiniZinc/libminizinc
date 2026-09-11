% The constraint `int_circuit`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_circuit` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% A FlatZinc array is one-based, so the numbering the successors are given in
% has to be passed explicitly rather than left implicit in an `index_set` the
% solver never receives. `list of` is what states the one-basing, and what the
% type-checker holds the caller to. The form the standard library's `circuit`
% calls forwards to it.

include "all_different.mzn";

predicate fzn_int_circuit(list of var int: xs, int: offset) =
  if length(xs) = 0 then
    true
  else
    let {
      int: n = length(xs);
      array [offset..offset + n - 1] of var int: x = array1d(offset..offset + n - 1, xs);
      array [offset..offset + n - 1] of var 1..n: order;
    } in all_different(x) /\
      all_different(order) /\
      forall (i in index_set(x)) (x[i] != i) /\
      order[offset] = 1 /\
      forall (i in index_set(x)) (
        order[x[i]] = if order[i] = n then 1 else order[i] + 1 endif
      )
  endif;
