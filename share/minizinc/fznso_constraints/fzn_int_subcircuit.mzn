% The constraint `int_subcircuit`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_subcircuit` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% A FlatZinc array is one-based, so the numbering the successors are given in
% has to be passed explicitly rather than left implicit in an `index_set` the
% solver never receives.

include "alldifferent.mzn";

predicate fzn_int_subcircuit(list of var int: xs, int: offset) =
  let {
    array [offset..offset + length(xs) - 1] of var int: x =
      array1d(offset..offset + length(xs) - 1, xs);
  } in (
    if length(x) = 0 then
      true
    else
      let {
        set of int: S = index_set(x);
        int: l = min(S);
        int: u = max(S);
        int: n = card(S);
        array [S] of var 1..n: order;
        array [S] of var bool: ins = array1d(S, [x[i] != i | i in S]);
        var l..u + 1: firstin = min([u + 1 + bool2int(ins[i]) * (i - u - 1) | i in S]);
        var S: lastin;
        var bool: empty = (firstin > u);
      } in alldifferent(x) /\
        alldifferent(order) /\
        (
          % If the subcircuit is empty then each node points at itself.
          %
          empty -> forall (i in S) (not ins[i])
        ) /\
        (
          % If the subcircuit is non-empty then order numbers the subcircuit.
          %
          (not empty) ->
            % The firstin node is numbered 1.
            order[firstin] = 1 /\
              % The lastin node is greater than firstin.
              lastin > firstin /\
              % The lastin node points at firstin.
              x[lastin] = firstin /\
              % And both are in
              ins[lastin] /\
              ins[firstin] /\
              % The successor of each node except where it is firstin is
              % numbered one more than the predecessor.
              forall (i in S) ((ins[i] /\ x[i] != firstin) -> order[x[i]] = order[i] + 1) /\
              % Each node that is not in is numbered after the lastin node.
              forall (i in S) (ins[i] \/ order[lastin] < order[i])
        )
    endif
  );
