include "fzn_all_different_int.mzn";
include "fzn_all_different_int_reif.mzn";
include "fzn_all_different_set.mzn";
include "fzn_all_different_set_reif.mzn";
include "analyse_all_different.mzn";

/** @group globals.alldifferent
  Constrain the array of integers \a x to be all different.
*/
predicate all_different(array[$X] of var int: x) =
   analyse_all_different(array1d(x)) /\
   fzn_all_different_int(array1d(x));

predicate all_different_reif(array[int] of var int: x, var bool: b) =
   fzn_all_different_int_reif(x, b);

/**  @group globals.alldifferent
  Constrain the array of sets of integers \a x to be all different.
*/
predicate all_different(array[$X] of var set of int: x) =
    fzn_all_different_set(array1d(x));
% Synonyms for the above.

predicate all_different_reif(array[$X] of var set of int: x, var bool: b) =
    fzn_all_different_set_reif(array1d(x), b);
% Synonyms for the above.

predicate alldifferent(array[$X] of var int: x) =
    all_different(array1d(x));

predicate alldifferent(array[$X] of var set of int: x) =
    all_different(array1d(x));

