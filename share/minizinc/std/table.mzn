include "table_bool.mzn";
include "table_int.mzn";

/**
  Represents the constraint \a x in \a t where we
  consider each row in \a t to be a tuple and \a t as a set of tuples.
*/
predicate table(array[int] of var bool: x, array[int, int] of bool: t) =
    table_bool(x, t);

/**
  Represents the constraint \a x in \a t where we
  consider each row in \a t to be a tuple and \a t as a set of tuples.
*/
predicate table(array[int] of var int: x, array[int, int] of int: t) =
    table_int(x, t);
