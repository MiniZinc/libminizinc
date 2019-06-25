/* Linearized version
 * Uses a predicate which constructs a subcircuit which always includes an extra dummy vertex
 * Is worse than the just slightly adapted standard variant...
*/
include "alldifferent.mzn";

/** @group globals
  Constrains the elements of \a x to define a subcircuit where \a x[\p i] = \p j
  means that \p j is the successor of \p i and \a x[\p i] = \p i means that \p i
  is not in the circuit.
*/
%% Linear version
predicate subcircuit(array[int] of var int: x) = 
    let { set of int: S = index_set(x),
          int: l = min(S),
          int: u = max(S),
          int: n = card(S),
          constraint forall(i in S)(x[i] in l..u),
          array[l..u+1] of var l..u+1: xx,
          constraint forall(i in S)(xx[i] in dom(x[i]) union {u+1}),         
    } in
    alldifferent(x) /\  
    subcircuit_wDummy(xx) /\
    forall( i in S, j in dom(x[i]) )(   %% also when i==j?
      eq_encode(x[i])[j] >= 2*eq_encode(xx[i])[j]
                            + eq_encode(xx[i])[u+1] + eq_encode(xx[u+1])[j] - 1      %% -1
      /\
      eq_encode(x[i])[j] >= eq_encode(xx[i])[j]
      /\
      eq_encode(x[i])[j] <= eq_encode(xx[i])[j] + eq_encode(xx[i])[u+1]
      /\
      eq_encode(x[i])[j] <= eq_encode(xx[i])[j] + eq_encode(xx[u+1])[j]
    )
    /\
    forall( i in S )(
      eq_encode(x[i])[i] == eq_encode(xx[i])[i]
    )
    ;

%% Should include at least 2 nodes if >0?
%% xx[n] is dummy
predicate subcircuit_wDummy(array[int] of var int: x) = 
    let { set of int: S = index_set(x),
          int: l = min(S),
          int: u = max(S),
          int: n = card(S),
          set of int: S__ = S diff {u},   %% the real nodes
          array[S__] of var 2..n: order,
%%          constraint order[n]==1,           %% fix the dummy
%%          var bool: empty = (firstin == u+1),  no, break 2-cycles with dummy
    } in
    alldifferent(x) /\  
 % NO   alldifferent(order) /\
    
      %%% MTZ model. Note that INTEGER order vars seem better!:
    forall( i in S__, j in dom(x[i]) where i!=j /\ j!=u )(
        order[i] - order[j] + (n-1)*eq_encode(x[i])[j]
  %      + (n-3)*bool2int(x[j]==i)     %% the Desrochers & Laporte '91 term
  %  --- strangely enough it is much worse on vrp-s2-v2-c7_vrp-v2-c7_det_ADAPT_1_INVERSE.mzn!
        <= n-2 ) /\

    %% Break 2-cycles with dummy:
    forall( i in S__ )(
      eq_encode(x[i])[u] + eq_encode(x[u])[i] <= 1
      /\
        %% Ensure dummy is in:
      if i in dom(x[i]) then
        eq_encode(x[i])[i] >= eq_encode(x[u])[u]
      else
        true
      endif
                 )
    /\
        
       % Symmetry? Each node that is not in is numbered after the lastin node.
       forall(i in S) (
    true
%           (not ins[i]) <-> (n == order[i])
       )
    ;

predicate subcircuit_reif(array[int] of var int: x, var bool: b) = 
    abort("Reified subcircuit/1 is not supported.");

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
