include "lex_lesseq_bool.mzn";
include "lex_lesseq_float.mzn";
include "lex_lesseq_int.mzn";
include "lex_lesseq_set.mzn";

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(array[int] of var bool: x,
                     array[int] of var bool: y) = 
    if length(x)=1 /\ length(y)=1 then x[min(index_set(x))] <= y[min(index_set(y))]
    elseif length(x)=0 then true
    elseif length(y)=0 then false
    else lex_lesseq_bool(x, y) endif;

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(array[int] of var float: x,
                     array[int] of var float: y) =
    if length(x)=1 /\ length(y)=1 then x[min(index_set(x))] <= y[min(index_set(y))]
    elseif length(x)=0 then true
    elseif length(y)=0 then false
    else lex_lesseq_float(x, y) endif;

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(array[int] of var int: x,
                     array[int] of var int: y) =
    if length(x)=1 /\ length(y)=1 then x[min(index_set(x))] <= y[min(index_set(y))]
    elseif length(x)=0 then true
    elseif length(y)=0 then false
    else lex_lesseq_int(x, y) endif;

/** @group globals.lexicographic
  Requires that the array \a x is lexicographically less than or equal to
  array \a y. Compares them from first to last element, regardless of indices.
*/
predicate lex_lesseq(array[int] of var set of int: x,
                     array[int] of var set of int: y) =
    if length(x)=1 /\ length(y)=1 then x[min(index_set(x))] <= y[min(index_set(y))]
    elseif length(x)=0 then true
    elseif length(y)=0 then false
    else lex_lesseq_set(x, y) endif;

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

