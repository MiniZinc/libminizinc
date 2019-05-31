include "fzn_value_precede_int.mzn";
include "fzn_value_precede_set.mzn";

/** @group globals.lexicographic
  Requires that \a s precede \a t in the array \a x.
  
  Precedence means that if any element of \a x
  is equal to \a t, then another element of \a x with a  lower index is equal
  to \a s.
*/
predicate value_precede(int: s, int: t, array[int] of var int: x) =
    fzn_value_precede_int(s, t, x);

/** @group globals.lexicographic
  Requires that \a s precede \a t in the array \a x.

  Precedence means that if an element of \a x
  contains \a t but not \a s, then another element of \a x with lower index contains
  \a s but not \a t.
*/
predicate value_precede(int: s, int: t, array[int] of var set of int: x) =
    fzn_value_precede_set(s, t, x);
