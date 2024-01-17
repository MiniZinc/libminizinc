include "strictly_increasing.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is in strict decreasing order
%-----------------------------------------------------------------------------%

predicate strictly_decreasing(array[$X] of var bool: x) =
    strictly_increasing(reverse(array1d(x)));

/** @group globals.sort Requires that the array \a x is in a stricly decreasing order (duplicates are *not* allowed). */
predicate strictly_decreasing(array[$X] of var opt int: x) =
    strictly_increasing(reverse(array1d(x)));

/** @group globals.sort Requires that the array \a x is in a stricly decreasing order (duplicates are *not* allowed). */
predicate strictly_decreasing(array[$X] of var opt float: x) =
    strictly_increasing(reverse(array1d(x)));

/** @group globals.sort Requires that the array \a x is in a stricly decreasing order (duplicates are *not* allowed). */
predicate strictly_decreasing(array[$X] of var set of int: x) =
    strictly_increasing(reverse(array1d(x)));
