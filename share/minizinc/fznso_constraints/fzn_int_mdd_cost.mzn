% The constraint `int_mdd_cost`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_mdd_cost` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

predicate fzn_int_mdd_cost(
  list of var int: xs,
  int: nodes,
  list of int: level,
  int: edges,
  list of int: from,
  list of set of int: label,
  list of int: weight,
  list of int: to,
  var int: total_cost,
) =
  let {
    set of int: NODE = 1..nodes;
    set of int: EDGE = 1..edges;
    int: L = length(xs);
    array [1..L] of int: maxlevelcost = [
      max (e in EDGE where level[from[e]] = l) (weight[e]) |
        l in 1..L,
    ];
    array [1..L] of int: minlevelcost = [
      min([0] ++ [weight[e] | e in EDGE where level[from[e]] = l /\ weight[e] < 0]) |
        l in 1..L,
    ];
    int: maxcost = sum(maxlevelcost);
    set of int: COST = sum(minlevelcost)..L * (maxcost + 1);
    array [0..nodes] of var bool: bn;
    array [EDGE] of var bool: be;
    array [0..nodes] of var COST: ln; % distance from T
    array [0..nodes] of var COST: un; % distance from root
  } in bn[0] /\ % true node is true
    bn[1] /\ % root must hold
    % T1 each node except the root enforces an outgoing edge
    forall (n in NODE) (bn[n] -> exists (e in EDGE where from[e] = n) (be[e])) /\
    % T23 each edge enforces its endpoints
    forall (e in EDGE) ((be[e] -> bn[to[e]]) /\ (be[e] -> bn[to[e]])) /\
    % T4 each edge enforces its label
    forall (e in EDGE) (be[e] -> xs[level[from[e]]] in label[e]) /\
    % P1 each node enforces its outgoing edges
    forall (e in EDGE) (bn[from[e]] /\ xs[level[from[e]]] in label[e] -> be[e]) /\
    % P2 each node except the root enforces an incoming edge
    exists (e in EDGE where to[e] = 0) (be[e]) /\
    forall (n in 2..nodes) (bn[n] -> exists (e in EDGE where to[e] = n) (be[e])) /\
    % P3 each label has a support
    forall (i in 1..L, d in dom(xs[i])) (
      xs[i] = d -> exists (e in EDGE where level[from[e]] = i /\ d in label[e]) (be[e])
    ) /\
    % P4 exactly one node at each level
    forall (i in 1..L) (sum (n in NODE where level[n] = i) (bn[n]) = 1) /\
    ln[0] = 0 /\
    un[1] = 0 /\
    forall (n in NODE) (
      ln[n] =
        min (e in EDGE where from[e] = n) (
          ln[to[e]] + weight[e] + (not be[e]) * (maxcost + 1 - weight[e])
        )
    ) /\
    forall (n in 2..nodes) (
      un[n] =
        min (e in EDGE where to[e] = n) (
          un[from[e]] + weight[e] + (not be[e]) * (maxcost + 1 - weight[e])
        )
    ) /\
    forall (e in EDGE) (be[e] -> un[from[e]] + weight[e] + ln[to[e]] <= maxcost) /\
    total_cost = ln[1];
