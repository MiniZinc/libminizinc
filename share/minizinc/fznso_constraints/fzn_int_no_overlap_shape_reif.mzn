% The constraint `int_no_overlap_shape_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_no_overlap_shape_reif`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

include "fzn_int_no_overlap_nd.mzn";

predicate fzn_int_no_overlap_shape_reif(
  int: dimensions,
  list of int: rect_size,
  list of int: rect_origin,
  list of set of int: shape,
  list of var int: position,
  list of var int: kind,
  var bool: b,
) =
  b <-> (
    assert(
      forall (ki in kind) (has_bounds(ki)),
      "int_no_overlap_shape: the `kind` argument must be bounded",
    ) /\
      let {
        set of int: DIMS = 1..dimensions;
        set of int: OBJECTS = 1..length(kind);
        int: rects = length(rect_size) div dimensions;
        array [1..rects, DIMS] of int: size = array2d(1..rects, DIMS, rect_size);
        array [1..rects, DIMS] of int: origin = array2d(1..rects, DIMS, rect_origin);
        array [OBJECTS, DIMS] of var int: x = array2d(OBJECTS, DIMS, position);
      } in forall (o1, o2 in OBJECTS where o1 < o2) (
        forall (s1 in dom(kind[o1]), s2 in dom(kind[o2])) ((
          kind[o1] = s1 /\ kind[o2] = s2 ->
            forall (r1 in shape[s1], r2 in shape[s2]) (
              fzn_int_no_overlap_nd(
                dimensions,
                [x[o1, j] + origin[r1, j] | j in DIMS] ++ [x[o2, j] + origin[r2, j] | j in DIMS],
                [size[r1, j] | j in DIMS] ++ [size[r2, j] | j in DIMS],
              )
            )
        ))
      )
  );
