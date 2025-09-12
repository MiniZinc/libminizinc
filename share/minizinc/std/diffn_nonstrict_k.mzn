include "fzn_diffn_nonstrict_k.mzn";
include "fzn_diffn_nonstrict_k_reif.mzn";

/** @group globals.packing
  Constrains \p k-dimensional boxes to be non-overlapping. For each box \p i
  and dimension \p j, \a box_posn[\p i, \p j] is the base position of the box
  in dimension \p j, and \a box_size[\p i, \p j] is the size in that dimension.
  Boxes whose size is 0 in at least one dimension can be packed anywhere.
*/
predicate diffn_nonstrict_k(
  array [$$E, $$F] of var int: box_posn,
  array [$$E, $$F] of var int: box_size,
) =
  assert(
    index_sets_agree(box_posn, box_size),
    "diffn: index sets of arguments are incorrect",
    fzn_diffn_nonstrict_k(index2int(box_posn), index2int(box_size)),
  );
