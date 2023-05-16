/* Constrain y to be the set of the non-absent elements in x */
predicate fzn_array_opt_int_union(array[int] of var opt int: x, var set of int: y) =
    if length(x) = 0 then y = {}
    elseif length(x) = 1 then
      (card(y) = occurs(x[min(index_set(x))])) /\
      (occurs(x[min(index_set(x))]) -> (deopt(x[min(index_set(x))]) in y))
    else
    y = array_union([let { var set of dom(x[i]): s;
                           constraint if occurs(x[i]) then deopt(x[i]) in s /\ card(s)=1 else card(s)=0 endif;
                         } in s | i in index_set(x)])
    endif;
