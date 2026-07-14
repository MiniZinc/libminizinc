predicate fzn_geost_smallest_bb(
  int: k,
  array [int, int] of int: rect_size,
  array [int, int] of int: rect_offset,
  array [int] of set of int: shape,
  array [int, int] of var int: xs,
  array [int] of var int: kind,
  array [int] of var int: lower_bounds,
  array [int] of var int: upper_bounds,
) =
  % Two useful definitions
  let {
    set of int: DIMS = 1..k;
    set of int: OBJECTS = index_set(kind);
  } in (
    % Posting the geost constraint
    fzn_geost_bb(k, rect_size, rect_offset, shape, xs, kind, lower_bounds, upper_bounds) /\
      % Posting the smallest bounding box constraints
      forall (j in DIMS) (
        % Lower boundary
        exists (o in OBJECTS, s in dom(kind[o])) (
          kind[o] = s /\ exists (r in shape[s]) (xs[o, j] + rect_offset[r, j] == lower_bounds[j])
        ) /\
          % Upper boundary
          exists (o in OBJECTS, s in dom(kind[o])) (
            kind[o] = s /\
              exists (r in shape[s]) (xs[o, j] + rect_offset[r, j] + rect_size[r, j] == upper_bounds[j])
          )
      )
  );
