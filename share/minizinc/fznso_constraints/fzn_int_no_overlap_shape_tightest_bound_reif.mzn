% The constraint `int_no_overlap_shape_tightest_bound_reif`, as the FZnSO
% registry defines it.
%
% This file is a solver's override point: declare
% `int_no_overlap_shape_tightest_bound_reif` and the body below is dropped in
% favour of the propagator, or ship a file of this name with a decomposition of
% your own.

include "fzn_int_no_overlap_shape_bounded.mzn";

predicate fzn_int_no_overlap_shape_tightest_bound_reif(
  int: dimensions,
  list of int: rect_size,
  list of int: rect_origin,
  list of set of int: shape,
  list of var int: position,
  list of var int: kind,
  list of var int: lower_bound,
  list of var int: upper_bound,
  var bool: b,
) =
  b <-> (
    let {
      set of int: DIMS = 1..dimensions;
      set of int: OBJECTS = 1..length(kind);
      int: rects = length(rect_size) div dimensions;
      array [1..rects, DIMS] of int: size = array2d(1..rects, DIMS, rect_size);
      array [1..rects, DIMS] of int: origin = array2d(1..rects, DIMS, rect_origin);
      array [OBJECTS, DIMS] of var int: x = array2d(OBJECTS, DIMS, position);
    } in fzn_int_no_overlap_shape_bounded(
      dimensions,
      rect_size,
      rect_origin,
      shape,
      position,
      kind,
      lower_bound,
      upper_bound,
    ) /\
      forall (j in DIMS) (
        exists (o in OBJECTS, s in dom(kind[o])) (
          kind[o] = s /\ exists (r in shape[s]) (x[o, j] + origin[r, j] = lower_bound[j])
        ) /\
          exists (o in OBJECTS, s in dom(kind[o])) (
            kind[o] = s /\
              exists (r in shape[s]) (x[o, j] + origin[r, j] + size[r, j] = upper_bound[j])
          )
      )
  );
