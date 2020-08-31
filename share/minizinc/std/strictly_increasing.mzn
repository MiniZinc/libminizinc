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
predicate strictly_increasing(array[int] of var bool: x) =
    analyse_all_different(x) /\
    fzn_strictly_increasing_bool(x);

predicate strictly_increasing_reif(array[int] of var bool: x, var bool: b) =
    fzn_strictly_increasing_bool_reif(x,b);

/** @group globals.sort Requires that the array \a x is in a stricly increasing order (duplicates are *not* allowed). */
predicate strictly_increasing(array[int] of var int: x) =
    analyse_all_different(x) /\
    fzn_strictly_increasing_int(x);

/** @group globals.sort Requires that the array \a x is in a stricly increasing order (duplicates are *not* allowed). */
predicate strictly_increasing(array[int] of var opt int: x) =
    analyse_all_different(x) /\
    fzn_strictly_increasing_int_opt(x);

predicate strictly_increasing_reif(array[int] of var int: x, var bool: b) =
    fzn_strictly_increasing_int_reif(x,b);

