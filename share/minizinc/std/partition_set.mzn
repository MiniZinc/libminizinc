include "fzn_partition_set.mzn";
include "fzn_partition_set_reif.mzn";

/** @group globals
  Constrains the sets in array \a S to partition the \a universe.
*/
predicate partition_set(array[int] of var set of int: S,
                        set of int: universe) =
    fzn_partition_set(S, universe);
