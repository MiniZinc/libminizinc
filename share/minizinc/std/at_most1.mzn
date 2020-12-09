include "fzn_at_most1.mzn";
include "fzn_at_most1_reif.mzn";

/**  @group globals.counting
  Requires that each pair of sets in \a s overlap in at most one element.
*/
predicate at_most1(array[$X] of var set of int: s) =
    fzn_at_most1(array1d(s));

predicate at_most1_reif(array[$X] of var set of int: s, var bool: b) =
    fzn_at_most1_reif(array1d(s), b);

% Synonym for the above.
predicate atmost1(array[$X] of var set of int: s) = at_most1(s);
