% The constraint `int_no_overlap_nd`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_no_overlap_nd` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% The k-dimensional form. A FlatZinc array is one-dimensional, so the box
% positions and extents are passed flat, one box's coordinates after another,
% with the number of dimensions alongside — `std` reads it off an `index_set`
% the solver never receives.
%
% `int_no_overlap` stays a separate constraint for two dimensions: a caller
% already has an array per coordinate there, and interleaving them would be
% building a copy of the model.

predicate fzn_int_no_overlap_nd(int: dimensions, list of var int: position, list of var int: size) =
  let {
    int: n = length(position) div dimensions;
  } in forall (b1, b2 in 1..n where b1 < b2) (
    exists (j in 1..dimensions) (
      let {
        int: p1 = (b1 - 1) * dimensions + j;
        int: p2 = (b2 - 1) * dimensions + j;
      } in position[p1] + size[p1] <= position[p2] \/ position[p2] + size[p2] <= position[p1]
    )
  );
