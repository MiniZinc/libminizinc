% `set_of_int_union` for a solver with no set variables. The registry names the
% result rather than returning it, so this states the equality; that lands on
% `set_of_int_eq`, which this layer also decomposes.

include "fznso_set_operations.mzn";

predicate fzn_set_of_int_union(
  var set of int: s,
  var set of int: t,
  var set of int: result,
) =
  result = fznso_nosets_set_union(s, t);
