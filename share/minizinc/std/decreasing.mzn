include "decreasing_bool.mzn";
include "decreasing_float.mzn";
include "decreasing_int.mzn";
include "decreasing_set.mzn";

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[$X] of var bool: x) =
    decreasing_bool(array1d(x));

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[$X] of var float: x) =
    decreasing_float(array1d(x));

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[$X] of var int: x) =
    decreasing_int(array1d(x));

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[$X] of var set of int: x) =
    decreasing_set(array1d(x));
