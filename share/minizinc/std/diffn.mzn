include "fzn_diffn.mzn";
include "fzn_diffn_reif.mzn";

/** @group globals.packing
  Constrains rectangles \p i, given by their origins (\a x[\p i], \a y[\p i])
  and sizes (\a dx[\p i], \a dy[\p i]), to be non-overlapping. Zero-width
  rectangles can still not overlap with any other rectangle.
*/
predicate diffn(
  array [$$E] of var int: x,
  array [$$E] of var int: y,
  array [$$E] of var int: dx,
  array [$$E] of var int: dy,
) =
  assert(
    index_sets_agree(x, y) /\ index_sets_agree(x, dx) /\ index_sets_agree(x, dy),
    "diffn: index set mismatch",
    fzn_diffn(index2int(x), index2int(y), index2int(dx), index2int(dy)),
  );

%-----------------------------------------------------------------------------%
