include "analyse_all_different.mzn";
include "fzn_symmetric_all_different.mzn";
include "fzn_symmetric_all_different_reif.mzn";

/** @group globals.alldifferent
  Requires the array of integers \a x to be all different, and for all \p i,
  \a x[\p i]=j \(\rightarrow\) \a x[\p j]=\p i.
*/
predicate symmetric_all_different(array[int] of var int:x) =
    analyse_all_different(x) /\
    fzn_symmetric_all_different(x);

predicate symmetric_all_different_reif(array[int] of var int:x, var bool: b) =
    fzn_symmetric_all_different_reif(x,b);
