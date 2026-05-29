/* Linearized version
 * Uses a predicate which constructs a subcircuit which always includes an extra dummy vertex
 * Is worse than the just slightly adapted standard variant...
*/
include "alldifferent.mzn";

%% Linear version
predicate subcircuit(
  array [int] of var int: xs,
) =
  let {
    set of int: S = index_set(xs);
    int: l = min(S);
    int: u = max(S);
    int: n = card(S);
    constraint forall (i in S) (xs[i] in l..u);
    array [l..u + 1] of var l..u + 1: xx;
    constraint forall (i in S) (xx[i] in dom(xs[i]) union {u + 1});
  } in alldifferent(xs) /\
    subcircuit_wDummy(xx) /\
    forall (
      i in S,
      j in dom(xs[i]), %% also when i==j?
    ) (
      eq_encode(xs[i])[j] >=
        2 * eq_encode(xx[i])[j] + eq_encode(xx[i])[u + 1] + eq_encode(xx[u + 1])[j] - 1 /\ %% -1
        eq_encode(xs[i])[j] >= eq_encode(xx[i])[j] /\
        eq_encode(xs[i])[j] <= eq_encode(xx[i])[j] + eq_encode(xx[i])[u + 1] /\
        eq_encode(xs[i])[j] <= eq_encode(xx[i])[j] + eq_encode(xx[u + 1])[j]
    ) /\
    forall (i in S) (eq_encode(xs[i])[i] == eq_encode(xx[i])[i]);

%% Should include at least 2 nodes if >0?
%% xx[n] is dummy
predicate subcircuit_wDummy(
  array [int] of var int: xs,
) =
  let {
    set of int: S = index_set(xs);
    int: l = min(S);
    int: u = max(S);
    int: n = card(S);
    set of int: S__ = S diff {u}; %% the real nodes
    array [S__] of var 2..n: order;
  } in %%          constraint order[n]==1,           %% fix the dummy
  %%          var bool: empty = (firstin == u+1),  no, break 2-cycles with dummy
  alldifferent(xs) /\
    % NO   alldifferent(order) /\
    %%% MTZ model. Note that INTEGER order vars seem better!:
    forall (i in S__, j in dom(xs[i]) where i != j /\ j != u) (
      order[i] - order[j] + (n - 1) * eq_encode(xs[i])[j] <=
        %      + (n-3)*bool2int(xs[j]==i)     %% the Desrochers & Laporte '91 term
        %  --- strangely enough it is much worse on vrp-s2-v2-c7_vrp-v2-c7_det_ADAPT_1_INVERSE.mzn!
        n - 2
    ) /\
    %% Break 2-cycles with dummy:
    forall (
      i in S__,
    ) (
      eq_encode(xs[i])[u] + eq_encode(xs[u])[i] <= 1 /\
        %% Ensure dummy is in:
        if i in dom(xs[i]) then eq_encode(xs[i])[i] >= eq_encode(xs[u])[u] else true endif
    ) /\
    % Symmetry? Each node that is not in is numbered after the lastin node.
    forall (i in S) (
      true
      %           (not ins[i]) <-> (n == order[i])
    );

predicate subcircuit_reif(array [int] of var int: xs, var bool: b) =
  abort("Reified subcircuit/1 is not supported.");

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
