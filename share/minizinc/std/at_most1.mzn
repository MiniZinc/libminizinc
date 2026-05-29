include "fzn_at_most1.mzn";
include "fzn_at_most1_reif.mzn";

/**  @group globals.counting
  Requires that each pair of sets in \a xs overlap in at most one element.
*/
predicate at_most1(array [$X] of var set of $$E: xs) = fzn_at_most1(enum2int(array1d(xs)));

% Synonym for the above.
predicate atmost1(array [$X] of var set of $$E: xs) = at_most1(xs);
