include "fzn_all_different_int.mzn";
include "fzn_all_different_set.mzn";
include "analyse_alldifferent.mzn";   

/** @group globals.alldifferent
  Constrain the array of integers \a x to be all different.
   */
predicate all_different(array[int] of var int: x) =
   analyse_alldifferent(x) /\  
   fzn_all_different_int(x);

/**  @group globals.alldifferent
  Constrain the array of sets of integers \a x to be all different.
*/
predicate all_different(array[int] of var set of int: x) =
    fzn_all_different_set(x);

% Synonyms for the above.

predicate alldifferent(array[int] of var int: x) =
    fzn_all_different_int(x);

predicate alldifferent(array[int] of var set of int: x) =
    fzn_all_different_set(x);

