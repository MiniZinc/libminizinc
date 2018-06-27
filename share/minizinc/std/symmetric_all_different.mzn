include "all_different.mzn";

/** @group globals.alldifferent
  Requires the array of integers \a x to be all different, and for all \p i,
  \a x[\p i]=j \(\rightarrow\) \a x[\p j]=\p i.
*/

predicate symmetric_all_different(array[int] of var int:x) =
    all_different(x)
/\  forall(i, j in index_set(x) where i!=j) (x[i] = j -> x[j] = i);
