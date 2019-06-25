predicate fzn_disjoint_reif(var set of int: s1, var set of int: s2, var bool: b) =
    b <-> s1 intersect s2 == {};
