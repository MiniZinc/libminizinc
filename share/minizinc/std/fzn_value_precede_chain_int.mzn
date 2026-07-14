include "seq_precede_chain.mzn";

predicate fzn_value_precede_chain_int(array [int] of int: order, array [int] of var int: xs) =
  if length(order) = 0 then
    true
  elseif min(index_set(order)) = 1 /\
    forall (i in index_set(order)) (order[i] = i) /\
     max(order) = ub_array(xs) then
    seq_precede_chain(xs)
  else
    let {
      int: offset = min(index_set(order)) - 1;
      int: l = lb_array(xs);
      int: u = ub_array(xs);
      array [1..u - l + 1] of int: p = [
        sum([i - offset | i in index_set(order) where order[i] = j]) |
          j in l..u,
      ];
      array [int] of var 0..length(order): Y =
        array1d(index_set(xs), [p[xs[i] - l + 1] | i in index_set(xs)]);
    } in seq_precede_chain(Y)
  endif;
