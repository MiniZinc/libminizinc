% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_diffn_k.mzn`, under the registry's name.

include "fzn_int_no_overlap_nd_reif.mzn";

predicate fzn_diffn_k_reif(
  array [int, int] of var int: box_posn,
  array [int, int] of var int: box_size,
  var bool: b,
) =
  fzn_int_no_overlap_nd_reif(card(index_set_2of2(box_posn)), array1d(box_posn), array1d(box_size), b);
