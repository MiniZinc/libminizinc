% The constraint `float_array_arg_max`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_array_arg_max` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% A FlatZinc array is one-based, so the index the caller numbers `xs` from is
% passed explicitly rather than left implicit in an `index_set` the solver never
% receives. `list of` is what states the one-basing, and what the type-checker
% holds the caller to.

predicate fzn_float_array_arg_max(list of var float: xs, int: offset, var int: index) =
  let {
    array [offset..offset + length(xs) - 1] of var float: x =
      array1d(offset..offset + length(xs) - 1, xs);
  } in (
    let {
      int: l = min(index_set(x));
      int: u = max(index_set(x));
      float: uy = ub_array(x);
    } in if exists (j in l..u) (lb(x[j]) = uy) then
      let {
        array [l..u] of var bool: d;
      } in % max is known to be uy
      x[index] = uy /\ % ith case must be equal to ub
        forall (j in l..u) (x[j] = uy -> index <= j) /\ % lower bound
        d[l] = (x[l] = uy) /\
        forall (j in l + 1..u) (d[j] <-> (d[j - 1] \/ (x[j] = uy))) /\
        forall (j in l..u) (not d[j] -> index >= j + 1) % upper bound
    else
      let {
        float: ly = lb_array(x);
        array [l..u] of var ly..uy: y;
        array [l..u] of var l..u: mi;
      } in y[l] = x[l] /\
        mi[l] = l /\
        index = mi[u] /\
        forall (j in l + 1..u) (
          y[j] == max(x[j], y[j - 1]) /\ mi[j] = if y[j - 1] >= x[j] then mi[j - 1] else j endif
        )
    endif
  );
