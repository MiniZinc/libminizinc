predicate fzn_disjoint_reif(var set of int: s, var set of int: t, var bool: b) =
  b <-> s intersect t == {};
