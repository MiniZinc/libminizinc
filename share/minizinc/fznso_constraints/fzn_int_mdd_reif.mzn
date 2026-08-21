% The constraint `int_mdd_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_mdd_reif` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_int_mdd_reif(
  list of var int: xs,
  int: nodes,
  list of int: level,
  int: edges,
  list of int: from,
  list of set of int: label,
  list of int: to,
  var bool: b,
) =
  let {
    set of int: NODE = 1..nodes;
    set of int: EDGE = 1..edges;
    int: L = length(xs);
    array [0..nodes] of var bool: bn;
    array [EDGE] of var bool: be;
    set of int: D = dom_array(xs);
  } in (
    b <-> bn[0] % true node is result
  ) /\
    bn[1] /\ % root is always true
    % T1 each node except the true enforces an outgoing edge
    forall (n in NODE) (bn[n] -> (exists (e in EDGE where from[e] = n) (be[e]) \/ b = 0)) /\
    % T23 each edge enforces its endpoints
    forall (e in EDGE) ((be[e] -> bn[from[e]]) /\ (be[e] -> bn[to[e]])) /\
    % T4 each edge enforces its label
    forall (e in EDGE) (be[e] -> xs[level[from[e]]] in label[e]) /\
    % P1 each node enforces its outgoing edges
    forall (e in EDGE) (bn[from[e]] /\ xs[level[from[e]]] in label[e] -> be[e]) /\
    (
      % P2 each node except the root enforces an incoming edge
      bn[0] -> exists (e in EDGE where to[e] = 0) (be[e])
    ) /\
    forall (n in 2..nodes) (bn[n] -> exists (e in EDGE where to[e] = n) (be[e])) /\
    % P3 each label has a support, either an edge or to give false
    forall (i in 1..L, d in D) (
      xs[i] = d ->
        forall (n in NODE where level[n] = i, e in EDGE where from[e] = n /\ d in label[e]) (
          bn[n] -> be[e]
        ) /\
          forall (
            n in NODE
               where level[n] = i /\ not exists (e in EDGE where from[e] = n) (d in label[e]),
          ) (bn[n] -> b = 0)
    ) /\
    % P4 at most one node at every level is true (redundant)
    forall (i in 1..L) (sum (n in NODE where level[n] = i) (bn[n]) <= 1);
