include "fzn_alldifferent_except_0.mzn";
include "fzn_alldifferent_except_0_reif.mzn";

/**  @group globals.alldifferent
  Constrain the elements of the array of integers \a vs to be pairwise different except for those
  elements that are assigned the value 0.
*/
predicate all_different_except_0(array[$X] of var int: vs) = 
	fzn_alldifferent_except_0(array1d(vs));

% Synonym for the above.
predicate alldifferent_except_0(array[$X] of var int: vs) = 
	all_different_except_0(vs);
