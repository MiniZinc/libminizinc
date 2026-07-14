include "path.mzn";

predicate fzn_bounded_path_reif(
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  var int: source,
  var int: sink,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var int: weight_bound,
  var bool: b,
) = b <-> (path(from, to, source, sink, ns, es) /\ weight_bound = sum (e in index_set(es)) (es[e] * w[e]));
