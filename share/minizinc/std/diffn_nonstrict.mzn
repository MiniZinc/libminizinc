/** @group globals.packing
  Constrains rectangles \p i, given by their origins (\a x[\p i], \a y[\p i])
  and sizes (\a dx[\p i], \a dy[\p i]), to be non-overlapping. Zero-width
  rectangles can be packed anywhere.
*/
predicate diffn_nonstrict(array[int] of var int: x,
                          array[int] of var int: y,
                          array[int] of var int: dx,
                          array[int] of var int: dy) =
    assert(
        index_set(x) = index_set(y)  /\
        index_set(x) = index_set(dx) /\
        index_set(x) = index_set(dy),
        "diffn: index set mismatch",
    forall(i,j in index_set(x) where i < j)(
        dx[i] = 0 \/ dx[j] = 0 \/ dy[i]=0 \/ dy[j]=0 \/
        x[i] + dx[i] <= x[j] \/ y[i] + dy[i] <= y[j] \/
        x[j] + dx[j] <= x[i] \/ y[j] + dy[j] <= y[i]
    )
    );
