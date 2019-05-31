/** @group globals.packing
  Constrains rectangles \p i, given by their origins (\a x[\p i], \a y[\p i])
  and sizes (\a dx[\p i], \a dy[\p i]), to be non-overlapping. Zero-width
  rectangles can still not overlap with any other rectangle.
*/
predicate diffn(array[int] of var int: x,
                array[int] of var int: y,
                array[int] of var int: dx,
                array[int] of var int: dy) =
    assert(
        index_set(x) = index_set(y)  /\
        index_set(x) = index_set(dx) /\
        index_set(x) = index_set(dy),
        "diffn: index set mismatch",
    fzn_diffn(x,y,dx,dy)
    );

include "fzn_diffn.mzn";

%-----------------------------------------------------------------------------%
