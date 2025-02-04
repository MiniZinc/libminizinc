predicate fzn_subgraph(
  array [int] of int: from,
  array [int] of int: to,
  array [int] of var bool: ns,
  array [int] of var bool: es,
) = forall (e in index_set(from)) ((es[e] -> ns[from[e]]) /\ (es[e] -> ns[to[e]]));

%-----------------------------------------------------------------------------%
