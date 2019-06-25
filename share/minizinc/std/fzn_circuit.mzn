include "all_different.mzn";

predicate fzn_circuit(array[int] of var int: x) = 
    let { set of int: S = index_set(x),
          int: l = min(S),
	  int: n = card(S),
	  array[S] of var 1..n: order
        } in
    all_different(x) /\	
    all_different(order) /\
    forall(i in S)(x[i] != i) /\ 
    order[l] = 1 /\
    forall(i in S)(order[i] != n -> order[x[i]] = order[i] + 1) /\
    forall(i in S)(order[i] == n -> x[i] = l );

%-----------------------------------------------------------------------------%
