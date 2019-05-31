include "fzn_increasing_bool.mzn";
include "fzn_increasing_float.mzn";
include "fzn_increasing_int.mzn";
include "fzn_increasing_set.mzn";

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[int] of var bool: x) =
    fzn_increasing_bool(x);

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[int] of var float: x) =
    fzn_increasing_float(x);

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[int] of var int: x) =
    fzn_increasing_int(x);

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array[int] of var set of int: x) =
    fzn_increasing_set(x);

