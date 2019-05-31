include "fzn_value_precede_chain_int.mzn";
include "fzn_value_precede_chain_set.mzn";

/** @group globals.lexicographic
  Requires that \a c[\p i] precedes \a c[\p i +1] in the array \a x.
  
  Precedence means that if any element of \a x
  is equal to \a \a c[\p i +1], then another element of \a x with a  lower index is equal
  to \a \a c[\p i].
*/
predicate value_precede_chain(array[int] of int: c, array[int] of var int: x) =
    fzn_value_precede_chain_int(c, x);

/** @group globals.lexicographic
  Requires that \a c[\p i] precedes \a c[\p i +1] in the array \a x.

  Precedence means that if an element of \a x
  contains \a \a c[\p i +1] but not \a \a c[\p i], then another element of \a x with lower index contains
  \a \a c[\p i] but not \a \a c[\p i +1].
*/
predicate value_precede_chain(array[int] of int: c, array[int] of var set of int: x) =
    fzn_value_precede_chain_set(c, x);
