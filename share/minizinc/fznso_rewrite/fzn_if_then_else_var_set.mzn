% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_if_then_else_var_set`; the registry names it for the
% type it takes and puts the value it determines last.
%
% `d[i]` is "no earlier condition held", so `c[i] /\ d[i]` picks out the one arm
% that decides the result. Typing the values `var` covers the constant case by
% narrowing, and `if_then_else_partiality` by being the same constraint.

include "fzn_set_of_int_if_then_else.mzn";

predicate fzn_if_then_else_var_set(
  array [int] of var bool: c,
  array [int] of var set of int: x,
  var set of int: y,
) = fzn_set_of_int_if_then_else(array1d(c), array1d(x), y);
