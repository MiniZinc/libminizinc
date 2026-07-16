include "fzn_distribute.mzn";
include "fzn_distribute_reif.mzn";

predicate fzn_distribute(
  array [int] of var int: count,
  array [int] of var int: cover,
  array [int] of var int: xs,
);

/** @group globals.counting
  Requires that \a count[\p i] is the number of occurrences of \a cover[\p i] in
  \a xs. The values in \a cover need not be distinct.
*/
predicate distribute(
  array [$X] of var int: count,
  array [$X] of var $$E: cover,
  array [$Y] of var $$E: xs,
) =
  assert(
    index_sets_agree(count, cover),
    "distribute: count and cover arrays must have identical index sets",
    fzn_distribute(array1d(count), array1d(enum2int(cover)), array1d(enum2int(xs))),
  );
