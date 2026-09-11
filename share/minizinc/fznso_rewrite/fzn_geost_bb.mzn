% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_geost.mzn`. This adds a bounding box the objects must fit inside.

include "fzn_int_no_overlap_shape_bounded.mzn";

predicate fzn_geost_bb(
  int: k,
  array [int, int] of int: rect_size,
  array [int, int] of int: rect_offset,
  array [int] of set of int: shape,
  array [int, int] of var int: xs,
  array [int] of var int: kind,
  array [int] of var int: lower_bound,
  array [int] of var int: upper_bound,
) =
  fzn_int_no_overlap_shape_bounded(
    k,
    array1d(rect_size),
    array1d(rect_offset),
    [{r - min(index_set_1of2(rect_size)) + 1 | r in s} | s in array1d(shape)],
    array1d(xs),
    array1d(kind),
    array1d(lower_bound),
    array1d(upper_bound),
  );
