include "table_bool.mzn";
include "table_int.mzn";

/** @group globals.extensional
  Represents the constraint \a x in \a t where we
  consider each row in \a t to be a tuple and \a t as a set of tuples.
*/
predicate table(array[int] of var bool: x, array[int, int] of bool: t) =
    assert (index_set_2of2(t) == index_set(x),
        "The second dimension of the table must equal the number of variables "
            ++ "in the first argument",
        table_bool(x, t)
    );

/** @group globals.extensional
  Represents the constraint \a x in \a t where we
  consider each row in \a t to be a tuple and \a t as a set of tuples.
*/
predicate table(array[int] of var int: x, array[int, int] of int: t) =
    assert (index_set_2of2(t) == index_set(x),
        "The second dimension of the table must equal the number of variables "
            ++ "in the first argument",
        table_int(x, t)
    );
