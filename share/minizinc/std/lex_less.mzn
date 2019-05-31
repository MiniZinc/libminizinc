include "lex_less_bool.mzn";
include "lex_less_float.mzn";
include "lex_less_int.mzn";
include "lex_less_set.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically less than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(array[int] of var bool: x,
                   array[int] of var bool: y) =
    lex_less_bool(x, y);

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically less than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(array[int] of var int: x,
                   array[int] of var int: y) =
    lex_less_int(x, y);

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically less than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(array[int] of var float: x,
                   array[int] of var float: y) =
    lex_less_float(x, y);

/** @group globals.lexicographic
  Requires that the array \a x is strictly lexicographically less than
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_less(array[int] of var set of int: x,
                   array[int] of var set of int: y) =
    lex_less_set(x, y);

% Alternative names for the above.
%
predicate lex_lt(array[int] of var bool: x, array[int] of var bool: y) =
	lex_less(x, y);

predicate lex_lt(array[int] of var int: x, array[int] of var int: y) =
	lex_less(x, y);

predicate lex_lt(array[int] of var float: x, array[int] of var float: y) =
	lex_less(x, y);

predicate lex_lt(array[int] of var set of int: x,
                  array[int] of var set of int: y) =
	lex_less(x, y);

