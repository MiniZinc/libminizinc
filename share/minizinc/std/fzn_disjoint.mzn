/** @group globals Requires that sets \a s1 and \a s2 do not intersect. */
predicate fzn_disjoint(var set of int: s1, var set of int: s2) =
    s1 intersect s2 == {};
