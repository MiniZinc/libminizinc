include "fzn_distribute.mzn";
include "fzn_distribute_reif.mzn";

predicate fzn_distribute(
  array [int] of var int: counts,
  array [int] of var int: cover,
  array [int] of var int: xs,
);

/** @group globals.counting
  Requires that \a counts[\p i] is the number of occurrences of \a cover[\p i] in
  \a xs. The values in \a cover need not be distinct.
*/
predicate distribute(
  array [$X] of var int: counts,
  array [$X] of var $$E: cover,
  array [$Y] of var $$E: xs,
) =
  assert(
    index_sets_agree(counts, cover),
    "distribute: counts and cover arrays must have identical index sets",
    fzn_distribute(array1d(counts), array1d(enum2int(cover)), array1d(enum2int(xs))),
  );
