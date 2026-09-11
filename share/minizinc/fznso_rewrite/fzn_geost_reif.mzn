% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_geost.mzn`, under the registry's name.

include "fzn_int_no_overlap_shape_reif.mzn";

predicate fzn_geost_reif(
  int: k,
  array [int, int] of int: rect_size,
  array [int, int] of int: rect_offset,
  array [int] of set of int: shape,
  array [int, int] of var int: xs,
  array [int] of var int: kind,
  var bool: b,
) =
  fzn_int_no_overlap_shape_reif(k,
    array1d(rect_size),
    array1d(rect_offset),
    [{r - min(index_set_1of2(rect_size)) + 1 | r in s} | s in array1d(shape)],
    array1d(xs),
    array1d(kind), b);
