include "fzn_increasing_bool.mzn";
include "fzn_increasing_bool.mzn";
include "fzn_increasing_bool_reif.mzn";
include "fzn_increasing_float.mzn";
include "fzn_increasing_float_reif.mzn";
include "fzn_increasing_float_opt.mzn";
include "fzn_increasing_float_opt_reif.mzn";
include "fzn_increasing_int.mzn";
include "fzn_increasing_int_reif.mzn";
include "fzn_increasing_int_opt.mzn";
include "fzn_increasing_int_opt_reif.mzn";
include "fzn_increasing_set.mzn";
include "fzn_increasing_set_reif.mzn";

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var bool: x) = fzn_increasing_bool(array1d(x));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var float: x) = fzn_increasing_float(array1d(x));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var opt float: x) = fzn_increasing_float_opt(array1d(x));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var $$E: x) = fzn_increasing_int(enum2int(array1d(x)));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var opt $$E: x) = fzn_increasing_int_opt(enum2int(array1d(x)));

/** @group globals.sort Requires that the array \a x is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var set of $$E: x) = fzn_increasing_set(enum2int(array1d(x)));
