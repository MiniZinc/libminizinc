% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_geost`; the registry names it for what it does — place
% objects whose *shape* is a decision — and for the type it takes.
%
% A FlatZinc array is one-dimensional, so the rectangle sizes, their origins and
% the object positions are passed flat, one row after another, with the number
% of dimensions alongside; `std` reads that off an `index_set` the solver never
% receives. The rectangle numbering `shape` refers to is rebased on the way in,
% since the registry gives it no offset of its own.

include "fzn_int_no_overlap_shape.mzn";

predicate fzn_geost(
  int: k,
  array [int, int] of int: rect_size,
  array [int, int] of int: rect_offset,
  array [int] of set of int: shape,
  array [int, int] of var int: xs,
  array [int] of var int: kind,
) =
  fzn_int_no_overlap_shape(
    k,
    array1d(rect_size),
    array1d(rect_offset),
    [{r - min(index_set_1of2(rect_size)) + 1 | r in s} | s in array1d(shape)],
    array1d(xs),
    array1d(kind),
  );
