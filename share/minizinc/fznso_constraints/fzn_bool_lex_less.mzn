% The constraint `bool_lex_less`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_lex_less` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_bool_lex_less(list of var bool: xs, list of var bool: ys) =
  lex_less_std_decomposition(xs, ys);
