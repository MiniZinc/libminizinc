% The constraint `int_no_overlap_shape_bounded`, as the FZnSO registry defines
% it.
%
% This file is a solver's override point: declare `int_no_overlap_shape_bounded`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

% See `fzn_geost.mzn`. This adds a bounding box the objects must fit inside.

include "fzn_int_no_overlap_shape.mzn";

predicate fzn_int_no_overlap_shape_bounded(
  int: dimensions,
  list of int: rect_size,
  list of int: rect_origin,
  list of set of int: shape,
  list of var int: position,
  list of var int: kind,
  list of var int: lower_bound,
  list of var int: upper_bound,
) =
  let {
    set of int: DIMS = 1..dimensions;
    set of int: OBJECTS = 1..length(kind);
    int: rects = length(rect_size) div dimensions;
    array [1..rects, DIMS] of int: size = array2d(1..rects, DIMS, rect_size);
    array [1..rects, DIMS] of int: origin = array2d(1..rects, DIMS, rect_origin);
    array [OBJECTS, DIMS] of var int: x = array2d(OBJECTS, DIMS, position);
  } in fzn_int_no_overlap_shape(dimensions, rect_size, rect_origin, shape, position, kind) /\
    forall (o in OBJECTS) (
      forall (s in dom(kind[o])) ((
        kind[o] = s ->
          forall (r in shape[s], j in DIMS) (
            x[o, j] + origin[r, j] >= lower_bound[j] /\
              x[o, j] + origin[r, j] + size[r, j] <= upper_bound[j]
          )
      ))
    );
