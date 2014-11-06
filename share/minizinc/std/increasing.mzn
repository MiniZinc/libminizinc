include "increasing_bool.mzn";
include "increasing_float.mzn";
include "increasing_int.mzn";
include "increasing_set.mzn";

/** Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[int] of var bool: x) =
    increasing_bool(x);

/** Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[int] of var float: x) =
    increasing_float(x);

/** Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[int] of var int: x) =
    increasing_int(x);

/** Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[int] of var set of int: x) =
    increasing_set(x);

