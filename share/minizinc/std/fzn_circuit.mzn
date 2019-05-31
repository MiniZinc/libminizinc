include "alldifferent.mzn";

/** @group globals
  Constrains the elements of \a x to define a circuit where \a x[\p i] = \p j means
  that \p j is the successor of \p i.
*/
predicate fzn_circuit(array[int] of var int: x) = 
    let { set of int: S = index_set(x),
          int: l = min(S),
	  int: n = card(S),
	  array[S] of var 1..n: order
        } in
    alldifferent(x) /\	
    alldifferent(order) /\
    forall(i in S)(x[i] != i) /\ 
    order[l] = 1 /\
    forall(i in S)(order[i] != n -> order[x[i]] = order[i] + 1) /\
    forall(i in S)(order[i] == n -> x[i] = l );

predicate fzn_circuit_reif(array[int] of var int: x, var bool: b) = 
    abort("Reified circuit/1 is not supported.");

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
