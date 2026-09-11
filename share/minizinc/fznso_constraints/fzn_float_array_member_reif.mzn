% The constraint `float_array_member_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_array_member_reif` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_float_array_member_reif(
  list of var float: xs,
  var float: value,
  var bool: b,
) =
  b <-> exists (i in index_set(xs)) (xs[i] == value);
