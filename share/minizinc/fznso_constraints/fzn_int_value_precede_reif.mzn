% The constraint `int_value_precede_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_value_precede_reif` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_int_value_precede_reif(int: s, int: t, list of var int: xs, var bool: b) =
  abort("Reified value_precede/3 for integers is not supported.");
