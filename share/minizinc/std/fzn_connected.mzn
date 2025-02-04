include "reachable.mzn";

predicate fzn_connected(
  array [int] of int: from,
  array [int] of int: to,
  array [int] of var bool: ns,
  array [int] of var bool: es,
) = let { var index_set(ns): r } in reachable(from, to, r, ns, es);

%-----------------------------------------------------------------------------%
