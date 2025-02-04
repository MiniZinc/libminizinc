include "reachable.mzn";

predicate fzn_dconnected(
  array [int] of int: from,
  array [int] of int: to,
  array [int] of var bool: ns,
  array [int] of var bool: es,
) = let { var index_set(ns): r } in dreachable(from, to, r, ns, es);

%-----------------------------------------------------------------------------%
