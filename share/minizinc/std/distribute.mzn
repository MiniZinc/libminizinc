include "fzn_distribute.mzn";
include "fzn_distribute_reif.mzn";

/** @group globals.counting
  Requires that \a card[\p i] is the number of occurrences of \a value[\p i] in
  \a base. The values in \a value need not be distinct.
*/
predicate distribute(array[$X] of var int: card,
                     array[$X] of var int: value,
                     array[$Y] of var int: base) =
    assert(index_sets_agree(card, value),
        "distribute: card and value arrays must have identical index sets",
        fzn_distribute(array1d(card), array1d(value), array1d(base))
    );
