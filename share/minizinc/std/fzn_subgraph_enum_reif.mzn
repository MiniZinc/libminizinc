predicate fzn_subgraph_reif(
  array [int] of int: from,
  array [int] of int: to,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var bool: b,
) = b <-> forall (e in index_set(from)) ((es[e] -> ns[from[e]]) /\ (es[e] -> ns[to[e]]));

%-----------------------------------------------------------------------------%
