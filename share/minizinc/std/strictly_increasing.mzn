include "fzn_strictly_increasing_int.mzn";
include "fzn_strictly_increasing_int_opt.mzn";
include "fzn_strictly_increasing_int_reif.mzn";
include "fzn_strictly_increasing_bool.mzn";
include "fzn_strictly_increasing_bool_reif.mzn";
include "analyse_all_different.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in strict increasing order
%-----------------------------------------------------------------------------%

/** @group globals.sort Requires that the array \a x is in a stricly increasing order (duplicates are *not* allowed). */
predicate strictly_increasing(array[$X] of var bool: x) =
    analyse_all_different(array1d(x)) /\
    fzn_strictly_increasing_bool(array1d(x));

predicate strictly_increasing_reif(array[$X] of var bool: x, var bool: b) =
    fzn_strictly_increasing_bool_reif(array1d(x),b);

/** @group globals.sort Requires that the array \a x is in a stricly increasing order (duplicates are *not* allowed). */
predicate strictly_increasing(array[$X] of var int: x) =
    analyse_all_different(array1d(x)) /\
    fzn_strictly_increasing_int(array1d(x));

/** @group globals.sort Requires that the array \a x is in a stricly increasing order (duplicates are *not* allowed). */
predicate strictly_increasing(array[$X] of var opt int: x) =
    analyse_all_different(array1d(x)) /\
    fzn_strictly_increasing_int_opt(array1d(x));

predicate strictly_increasing_reif(array[$X] of var int: x, var bool: b) =
    fzn_strictly_increasing_int_reif(array1d(x),b);

