include "fzn_partition_set.mzn";
include "fzn_partition_set_reif.mzn";

predicate fzn_partition_set(array [int] of var set of int: xs, set of int: universe);

/** @group globals.set
  Constrains the sets in array \a xs to partition the \a universe.
*/
predicate partition_set(array [$X] of var set of $$E: xs, set of $$E: universe) =
  fzn_partition_set(array1d(enum2int(xs)), enum2int(universe));
