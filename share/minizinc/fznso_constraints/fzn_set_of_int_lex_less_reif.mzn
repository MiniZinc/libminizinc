% The constraint `set_of_int_lex_less_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `set_of_int_lex_less_reif` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_set_of_int_lex_less_reif(
  list of var set of int: xs,
  list of var set of int: ys,
  var bool: b,
) =
  b <-> lex_less_std_decomposition(xs, ys);
