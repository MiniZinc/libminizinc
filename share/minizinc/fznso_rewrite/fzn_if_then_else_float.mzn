% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The constant-valued form. It is the same constraint: a solver declares the
% values `par` if that is all its propagator takes, and the compiler generates
% the dispatch. See `fzn_if_then_else_var_float.mzn`.

include "fzn_float_if_then_else.mzn";

predicate fzn_if_then_else_float(array [int] of var bool: c, array [int] of float: x, var float: y) =
  fzn_float_if_then_else(array1d(c), array1d(x), y);
