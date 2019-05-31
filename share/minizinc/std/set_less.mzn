include "fzn_set_less.mzn";

/** @group globals.lexicographic
  Requires that the set \a x is strictly lexicographically less than
  set \a y. Compares them element wise
*/
predicate set_less(var set of int: x,
                   var set of int: y) =
    fzn_set_less(x, y);

% Alternative names for the above.
%
predicate set_lt(var set of int: x, var set of int: y) =
	set_less(x, y);
