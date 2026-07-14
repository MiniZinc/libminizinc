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

predicate fzn_increasing_bool(array [int] of var bool: xs);
predicate fzn_increasing_float(array [int] of var float: xs);
predicate fzn_increasing_float_opt(array [int] of var opt float: xs);
predicate fzn_increasing_int(array [int] of var int: xs);
predicate fzn_increasing_int_opt(array [int] of var opt int: xs);
predicate fzn_increasing_set(array [int] of var set of int: xs);

/** @group globals.sort Requires that the array \a xs is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var bool: xs) = fzn_increasing_bool(array1d(xs));

/** @group globals.sort Requires that the array \a xs is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var float: xs) = fzn_increasing_float(array1d(xs));

/** @group globals.sort Requires that the array \a xs is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var opt float: xs) = fzn_increasing_float_opt(array1d(xs));

/** @group globals.sort Requires that the array \a xs is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var $$E: xs) = fzn_increasing_int(enum2int(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var opt $$E: xs) = fzn_increasing_int_opt(enum2int(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in increasing order (duplicates are allowed). */
predicate increasing(array [$X] of var set of $$E: xs) = fzn_increasing_set(enum2int(array1d(xs)));
