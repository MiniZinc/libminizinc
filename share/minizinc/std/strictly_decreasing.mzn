include "strictly_increasing.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is in strict decreasing order
%-----------------------------------------------------------------------------%

predicate strictly_decreasing(array [$X] of var bool: xs) = strictly_increasing(reverse(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in a strictly decreasing order (duplicates are *not* allowed). */
predicate strictly_decreasing(array [$X] of var opt $$E: xs) =
  strictly_increasing(reverse(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in a strictly decreasing order (duplicates are *not* allowed). */
predicate strictly_decreasing(array [$X] of var opt float: xs) =
  strictly_increasing(reverse(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in a strictly decreasing order (duplicates are *not* allowed). */
predicate strictly_decreasing(array [$X] of var set of $$E: xs) =
  strictly_increasing(reverse(array1d(xs)));
