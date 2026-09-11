% The constraint `int_lex_less`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_lex_less` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_int_lex_less(list of var int: xs, list of var int: ys) =
  lex_less_std_decomposition(xs, ys);
