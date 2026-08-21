% The constraint `int_array_union`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_array_union` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_int_array_union(list of var int: xs, var set of int: result) =
  result =
    array_union([
      let {
        var set of dom(xs[i]): s;
        constraint xs[i] in s /\ card(s) = 1;
      } in s |
        i in index_set(xs),
    ]);
