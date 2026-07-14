include "path.mzn";

predicate fzn_bounded_dpath_reif(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  var int: source,
  var int: sink,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var int: weight_bound,
  var bool: b,
) = b <-> (dpath(N, E, from, to, source, sink, ns, es) /\ weight_bound = sum (e in 1..E) (es[e] * w[e]));
