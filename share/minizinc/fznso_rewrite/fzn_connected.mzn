% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_connected`; the registry names it for the type it takes —
% a graph — and puts the arrays a solver receives in the order it gives them.
%
% The nodes are numbered by `nodes`'s index set, which a FlatZinc array cannot
% carry, so the index it is numbered from travels alongside as a plain `int`.
% `from` and `to` hold node numbers in that same numbering.

include "fzn_graph_connected.mzn";
include "reachable.mzn";

predicate fzn_connected(
  array [int] of int: from,
  array [int] of int: to,
  array [int] of var bool: ns,
  array [int] of var bool: es,
) =
  fzn_graph_connected(array1d(from), array1d(to), min(index_set(ns)), array1d(ns), array1d(es));
