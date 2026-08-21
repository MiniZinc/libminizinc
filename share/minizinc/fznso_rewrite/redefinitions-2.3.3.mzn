% Shadows `std`'s file of the same name. A set of floats is a union of ranges,
% so membership is a disjunction of the registry's `float_in` over them — which
% is the one place a solver gets to implement it.

include "fzn_float_in.mzn";

predicate float_set_in(var float: x, set of float: s) =
  let {
    array [int] of float: r1d = set_to_ranges(s);
    array [int, 1..2] of float: r = array2d(1..length(r1d) div 2, 1..2, r1d);
  } in exists (i in index_set_1of2(r)) (fzn_float_in(x, r[i, 1], r[i, 2]));
