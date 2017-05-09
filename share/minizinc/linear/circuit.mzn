include "alldifferent.mzn";

/** @group globals
  Constrains the elements of \a x to define a circuit where \a x[\p i] = \p j means
  that \p j is the successor of \p i.
*/
% Linear version.
predicate circuit(array[int] of var int: x) = 
    let { set of int: S = index_set(x),
          int: l = min(S),
	  int: n = card(S),
	  array[l+1..l+n-1] of var 2..n: order
        } in
    alldifferent(x) /\	
    forall(i in S)(x[i] != i) /\ 
%    alldifferent(order) /\
    %%% MTZ model. Note that INTEGER order vars seem better!:
    forall (i,j in l+1..l+n-1 where i!=j /\ j in dom(x[i])) (
      order[i] - order[j] + (n-1)* bool2int(x[i]==j)
        + (n-3)*bool2int(x[j]==i)     %% the Desrochers & Laporte '91 term
  %%%%  --- strangely enough it is much worse on vrp-s2-v2-c7_vrp-v2-c7_det_ADAPT_1_INVERSE.mzn!
        <= n-2 )
    %% ... but seems improved with this:
    /\ if n>2 then forall (i,j in S where i<j) ( x[i]!=j \/ x[j]!=i )      %% Only this improves overall with DL'91
       else true endif
    ;

predicate circuit_reif(array[int] of var int: x, var bool: b) = 
    abort("Reified circuit/1 is not supported.");

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
