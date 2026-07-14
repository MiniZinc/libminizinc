include "tree.mzn";

predicate fzn_steiner(
  int: N,
  int: E,
  array [int] of int: from,
  array [int] of int: to,
  array [int] of int: w,
  array [int] of var bool: ns,
  array [int] of var bool: es,
  var int: weight_bound,
) = let { var 1..N: r } in tree(N, E, from, to, r, ns, es) /\ weight_bound = sum (e in 1..E) (es[e] * w[e]);

%-----------------------------------------------------------------------------%
