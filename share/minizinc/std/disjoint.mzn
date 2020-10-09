include "fzn_disjoint.mzn";
include "fzn_disjoint_reif.mzn";

/** @group globals Requires that sets \a s1 and \a s2 do not intersect. */
predicate disjoint(var set of int: s1, var set of int: s2) =
    fzn_disjoint(s1,s2);
