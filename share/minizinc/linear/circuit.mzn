include "alldifferent.mzn";

/** @group globals
  Constrains the elements of \a x to define a circuit where \a x[\p i] = \p j means
  that \p j is the successor of \p i.
*/
% Linear version.
predicate circuit(array[int] of var int: x) = 
    let { set of int: S = index_set(x),
          int: l = min(S),
          int: u = max(S),
	  int: n = card(S),
	  constraint forall( i in S )( x[i] in S diff {i} ),             %% Self-mapping and exclude i->i before alldifferent
        } in
    alldifferent(x) /\	
%    alldifferent(order) /\
    if nMZN__fSECcuts>0 then
      let {
        array [int, int] of var int: eq_x = eq_encode( x ),
        constraint assert( l==min( index_set_2of2( eq_x ) ), "circuit: index set mismatch" ),   %% self-mapping
        constraint assert( u==max( index_set_2of2( eq_x ) ), "circuit: index set mismatch" ),
      } in
      circuit__SECcuts( eq_x )
    else true endif /\
    if nMZN__fSECcuts<2 then
      %%% MTZ model. Note that INTEGER order vars seem better!:
      let {
        array[l+1..l+n-1] of var 2..n: order,
      } in
      forall (i,j in l+1..l+n-1 where i!=j /\ j in dom(x[i])) (
        order[i] - order[j] + (n-1)* bool2int(x[i]==j)
          + (n-3)*bool2int(x[j]==i)     %% the Desrochers & Laporte '91 term
          %%%%  --- strangely enough it is much worse on vrp-s2-v2-c7_vrp-v2-c7_det_ADAPT_1_INVERSE.mzn!
          <= n-2 )
    else true endif
    %% ... but seems improved with this (leaving also for SEC)
    /\ if n>2 then forall (i,j in S where i<j) ( x[i]!=j \/ x[j]!=i )      %% Only this improves overall with DL'91
       else true endif
    ;

predicate circuit_reif(array[int] of var int: x, var bool: b) = 
    abort("Reified circuit/1 is not supported.");

%-----------------------------------------------------------------------------%
predicate circuit__SECcuts(array[int,int] of var int: x);     %% passed on
%-----------------------------------------------------------------------------%
