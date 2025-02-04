include "fzn_partition_set.mzn";
include "fzn_partition_set_reif.mzn";

/** @group globals.set
  Constrains the sets in array \a S to partition the \a universe.
*/
predicate partition_set(array [$X] of var set of $$E: S, set of $$E: universe) =
  fzn_partition_set(array1d(enum2int(S)), enum2int(universe));
