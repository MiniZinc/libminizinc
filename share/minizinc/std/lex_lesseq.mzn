include "fzn_lex_lesseq_bool.mzn";
include "fzn_lex_lesseq_float.mzn";
include "fzn_lex_lesseq_int.mzn";
include "fzn_lex_lesseq_set.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(array[int] of var bool: x,
                     array[int] of var bool: y) = 
    fzn_lex_lesseq_bool(x, y);

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(array[int] of var float: x,
                     array[int] of var float: y) =
    fzn_lex_lesseq_float(x, y);

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(array[int] of var int: x,
                     array[int] of var int: y) =
    fzn_lex_lesseq_int(x, y);

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(array[int] of var set of int: x,
                     array[int] of var set of int: y) =
    fzn_lex_lesseq_set(x, y);

% Alternative names for the above.
%
predicate lex_leq(array[int] of var bool: x, array[int] of var bool: y) =
	lex_lesseq(x, y);

predicate lex_leq(array[int] of var int: x, array[int] of var int: y) =
	lex_lesseq(x, y);

predicate lex_leq(array[int] of var float: x, array[int] of var float: y) =
	lex_lesseq(x, y);

predicate lex_leq(array[int] of var set of int: x,
                  array[int] of var set of int: y) =
	lex_lesseq(x, y);

