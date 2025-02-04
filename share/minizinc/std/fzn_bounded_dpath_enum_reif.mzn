include "path.mzn";

predicate fzn_bounded_dpath_reif(
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  var int: s,
  var int: t,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var int: K,
  var bool: b,
) = b <-> (dpath(from, to, s, t, ns, es) /\ K = sum (e in index_set(es)) (es[e] * w[e]));
