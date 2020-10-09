include "fzn_alldifferent_except.mzn";
include "fzn_alldifferent_except_reif.mzn";

/**  @group globals.alldifferent
  Constrain the array of integers \a vs to be all different except those
  elements that appear in the set \a S.
*/
predicate alldifferent_except(array[$X] of var int: vs, set of int: S) = 
   fzn_alldifferent_except(array1d(vs),S);

predicate alldifferent_except_reif(array[$X] of var int: vs, set of int: S, var bool: b) = 
   fzn_alldifferent_except_reif(array1d(vs),S,b);

