% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The k-dimensional form. A FlatZinc array is one-dimensional, so the box
% positions and extents are passed flat, one box's coordinates after another,
% with the number of dimensions alongside — `std` reads it off an `index_set`
% the solver never receives.
%
% `int_no_overlap` stays a separate constraint for two dimensions: a caller
% already has an array per coordinate there, and interleaving them would be
% building a copy of the model.

include "fzn_int_no_overlap_nd.mzn";

predicate fzn_diffn_k(array [int, int] of var int: box_posn, array [int, int] of var int: box_size) =
  fzn_int_no_overlap_nd(
    card(index_set_2of2(box_posn)),
    array1d(box_posn),
    array1d(box_size),
  );
