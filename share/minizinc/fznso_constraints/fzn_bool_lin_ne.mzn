% The constraint `bool_lin_ne`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_lin_ne` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% The registry's name for the last hole in the type × operation matrix. There is
% no `bool_lin_ne` builtin, so the fallback is the integer one over the same
% coefficients: a Boolean linear sum is an integer one whose variables are 0/1.
%
% The total is a variable here, not a constant. An integer total folds into the
% coefficient list — `∑cᵢxᵢ = t` is `int_lin_eq(c ++ [-1], x ++ [t], 0)` — but a
% Boolean one cannot, so this entry has to name it.

predicate fzn_bool_lin_ne(list of int: coeffs, list of var bool: xs, var int: total) =
  int_lin_ne(coeffs ++ [-1], [bool2int(x) | x in xs] ++ [total], 0);
