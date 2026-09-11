% The constraint `float_if_then_else`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_if_then_else` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% `d[i]` is "no earlier condition held", so `c[i] /\ d[i]` picks out the one arm
% that decides the result. Typing the values `var` covers the constant case by
% narrowing, and `if_then_else_partiality` by being the same constraint.

predicate fzn_float_if_then_else(
  list of var bool: conditions,
  list of var float: values,
  var float: result,
) =
  let {
    array [index_set(conditions)] of var bool: d;
  } in forall (i in index_set(conditions)) (
    if i > 1 then d[i] = (not conditions[i - 1] /\ d[i - 1]) else d[i] = true endif
  ) /\
    forall (i in index_set(conditions)) (conditions[i] /\ d[i] -> result = values[i]);
