include "fzn_decreasing_bool.mzn";
include "fzn_decreasing_bool_reif.mzn";
include "fzn_decreasing_float.mzn";
include "fzn_decreasing_float_reif.mzn";
include "fzn_decreasing_int.mzn";
include "fzn_decreasing_int_reif.mzn";
include "fzn_decreasing_set.mzn";
include "fzn_decreasing_set_reif.mzn";

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[$X] of var bool: x) =
	fzn_decreasing_bool(array1d(x));

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[$X] of var float: x) =
	fzn_decreasing_float(array1d(x));

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[$X] of var int: x) =
  fzn_decreasing_int(array1d(x));

/** @group globals.sort Requires that the array \a x is in decreasing order (duplicates are allowed). */
predicate decreasing(array[$X] of var set of int: x) =
  fzn_decreasing_set(array1d(x));
