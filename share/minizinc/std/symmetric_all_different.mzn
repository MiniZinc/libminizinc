include "analyse_alldifferent.mzn";
include "fzn_symmetric_all_different.mzn";

/** @group globals.alldifferent
  Requires the array of integers \a x to be all different, and for all \p i,
  \a x[\p i]=j \(\rightarrow\) \a x[\p j]=i.
*/

predicate symmetric_all_different(array[int] of var int:x) =
	analyse_alldifferent(x) /\
	fzn_symmetric_all_different(x);
