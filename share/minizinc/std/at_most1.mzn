include "fzn_at_most1.mzn";
include "fzn_at_most1_reif.mzn";

/**  @group globals.counting
  Requires that each pair of sets in \a s overlap in at most one element.
*/
predicate at_most1(array[int] of var set of int: s) =
    fzn_at_most1(s);

predicate at_most1_reif(array[int] of var set of int: s, var bool: b) =
    fzn_at_most1_reif(s, b);

% Synonym for the above.
predicate atmost1(array[int] of var set of int: s) = fzn_at_most1(s);
