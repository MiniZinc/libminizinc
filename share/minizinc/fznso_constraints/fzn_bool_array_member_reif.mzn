% The constraint `bool_array_member_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_array_member_reif` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_bool_array_member_reif(
  list of var bool: xs,
  var bool: value,
  var bool: b,
) =
  b <-> exists (i in index_set(xs)) (xs[i] == value);
