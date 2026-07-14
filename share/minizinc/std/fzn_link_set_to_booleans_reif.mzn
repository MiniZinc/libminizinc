predicate fzn_link_set_to_booleans_reif(
  var set of int: s,
  array [int] of var bool: bs,
  var bool: bb,
) = bb <-> forall (i in index_set(bs)) (bs[i] <-> i in s);
