include "tree.mzn";
include "subgraph.mzn";

predicate fzn_dpath(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  var int: source,
  var int: sink,
  array [int] of var bool: ns,
  array [int] of var bool: es,
) = dtree(N, E, from, to, source, ns, es) /\ dtree(N, E, to, from, sink, ns, es);

%-----------------------------------------------------------------------------%
