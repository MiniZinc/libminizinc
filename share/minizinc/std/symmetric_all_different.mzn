include "analyse_all_different.mzn";
include "fzn_symmetric_all_different.mzn";
include "fzn_symmetric_all_different_reif.mzn";

/** @group globals.alldifferent
  Requires the array of integers \a xs to be all different, and for all \p i,
  \a xs[\p i]=\p j \(\rightarrow\) \a xs[\p j]=\p i.
*/
predicate symmetric_all_different(array [int] of var int: xs) =
  analyse_all_different(xs) /\ fzn_symmetric_all_different(xs);
