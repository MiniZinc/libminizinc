include "all_different_int.mzn";
include "all_different_set.mzn";
include "analyse_alldifferent.mzn";   

/** @group globals.alldifferent
  Constrain the array of integers \a x to be all different.
*/
predicate all_different(array[$X] of var int: x) =
   analyse_alldifferent(array1d(x)) /\  
   all_different_int(array1d(x));

/**  @group globals.alldifferent
  Constrain the array of sets of integers \a x to be all different.
*/
predicate all_different(array[$X] of var set of int: x) =
    all_different_set(array1d(x));
% Synonyms for the above.

predicate alldifferent(array[$X] of var int: x) =
    all_different_int(array1d(x));

predicate alldifferent(array[$X] of var set of int: x) =
    all_different_set(array1d(x));

