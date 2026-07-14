include "all_different.mzn";

predicate fzn_circuit(array [int] of var int: xs) =
  if length(xs) = 0 then
    true
  else
    let {
      set of int: S = index_set(xs);
      int: l = min(S);
      int: n = card(S);
      array [S] of var 1..n: order;
    } in all_different(xs) /\
      all_different(order) /\
      forall (i in S) (xs[i] != i) /\
      order[l] = 1 /\
      %forall(i in S)(order[i] != n -> order[x[i]] = order[i] + 1) /\
      %forall(i in S)(order[i] == n -> x[i] = l );
      forall (i in S) (order[xs[i]] = if order[i] = n then 1 else order[i] + 1 endif)
  endif;

%-----------------------------------------------------------------------------%
