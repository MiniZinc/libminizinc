include "decreasing_bool.mzn";
include "decreasing_float.mzn";
include "decreasing_int.mzn";
include "decreasing_set.mzn";

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[int] of var bool: x) =
    decreasing_bool(x);

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[int] of var float: x) =
    decreasing_float(x);

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[int] of var int: x) =
    decreasing_int(x);

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[int] of var set of int: x) =
    decreasing_set(x);
