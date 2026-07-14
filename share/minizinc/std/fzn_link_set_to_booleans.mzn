predicate fzn_link_set_to_booleans(var set of int: s, array [int] of var bool: bs) =
  forall (i in index_set(bs)) (bs[i] <-> i in s);
