include "fzn_decreasing_bool.mzn";
include "fzn_decreasing_float.mzn";
include "fzn_decreasing_int.mzn";
include "fzn_decreasing_set.mzn";

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[int] of var bool: x) =
    fzn_decreasing_bool(x);

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[int] of var float: x) =
    fzn_decreasing_float(x);

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[int] of var int: x) =
    fzn_decreasing_int(x);

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[int] of var set of int: x) =
    fzn_decreasing_set(x);

