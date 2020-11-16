include "increasing_bool.mzn";
include "increasing_float.mzn";
include "increasing_int.mzn";
include "increasing_set.mzn";

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[$X] of var bool: x) =
    increasing_bool(array1d(x));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[$X] of var float: x) =
    increasing_float(array1d(x));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[$X] of var int: x) =
    increasing_int(array1d(x));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[$X] of var set of int: x) =
    increasing_set(array1d(x));

