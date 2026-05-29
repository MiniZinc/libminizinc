include "increasing.mzn";

/** @group globals.sort Requires that the array \a xs is in decreasing order (duplicates are allowed). */
predicate decreasing(array [$X] of var bool: xs) = increasing(reverse(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in decreasing order (duplicates are allowed). */
predicate decreasing(array [$X] of var opt float: xs) = increasing(reverse(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in decreasing order (duplicates are allowed). */
predicate decreasing(array [$X] of var opt $$E: xs) = increasing(reverse(array1d(xs)));

/** @group globals.sort Requires that the array \a xs is in decreasing order (duplicates are allowed). */
predicate decreasing(array [$X] of var set of $$E: xs) = increasing(reverse(array1d(xs)));
