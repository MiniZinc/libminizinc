include "fzn_alldifferent_except_0.mzn";
include "fzn_alldifferent_except_0_reif.mzn";

/**  @group globals.alldifferent
  Constrain the array of integers \a vs to be all different except those
  elements that are assigned the value 0.
*/
predicate alldifferent_except_0(array[$X] of var int: vs) = 
   fzn_alldifferent_except_0(array1d(vs));

predicate alldifferent_except_0_reif(array[$X] of var int: vs, var bool: b) = 
   fzn_alldifferent_except_0_reif(array1d(vs),b);
