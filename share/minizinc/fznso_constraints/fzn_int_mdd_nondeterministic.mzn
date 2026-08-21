% The constraint `int_mdd_nondeterministic`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_mdd_nondeterministic` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_int_mdd_nondeterministic(
  list of var int: xs,
  int: nodes,
  list of int: level,
  int: edges,
  list of int: from,
  list of set of int: label,
  list of int: to,
) =
  let {
    set of int: NODE = 1..nodes;
    set of int: EDGE = 1..edges;
    int: L = length(xs);
    array [0..nodes] of var bool: bn;
    array [EDGE] of var bool: be;
    set of int: D = dom_array(xs);
  } in bn[0] /\ % true node is true
    bn[1] /\ % root must hold
    % T1 each node except the root enforces an outgoing edge
    forall (n in NODE) (bn[n] -> exists (e in EDGE where from[e] = n) (be[e])) /\
    % T23 each edge enforces its endpoints
    forall (e in EDGE) ((be[e] -> bn[from[e]]) /\ (be[e] -> bn[to[e]])) /\
    % T4 each edge enforces its label
    forall (e in EDGE) (be[e] -> xs[level[from[e]]] in label[e]) /\
    % P2 each node except the root enforces an incoming edge
    exists (e in EDGE where to[e] = 0) (be[e]) /\
    forall (n in 2..nodes) (bn[n] -> exists (e in EDGE where to[e] = n) (be[e])) /\
    % P3 each label has a support
    forall (i in 1..L, d in D) (
      xs[i] = d -> exists (e in EDGE where level[from[e]] = i /\ d in label[e]) (be[e])
    );
