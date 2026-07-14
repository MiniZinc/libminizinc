predicate fzn_alldifferent_except_reif(array [int] of var int: xs, set of int: except, var bool: b) =
  b <-> forall (i, j in index_set(xs) where i < j) ((xs[i] in except /\ xs[j] in except) \/ xs[i] != xs[j]);
