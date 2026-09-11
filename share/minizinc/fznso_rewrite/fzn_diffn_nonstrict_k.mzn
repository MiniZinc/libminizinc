% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% See `fzn_diffn_k.mzn`. This is the form in which a box of zero extent in some
% dimension overlaps nothing.

include "fzn_int_no_overlap_nd_nonstrict.mzn";

predicate fzn_diffn_nonstrict_k(
  array [int, int] of var int: box_posn,
  array [int, int] of var int: box_size,
) =
  fzn_int_no_overlap_nd_nonstrict(
    card(index_set_2of2(box_posn)),
    array1d(box_posn),
    array1d(box_size),
  );
