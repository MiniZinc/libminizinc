/* Constrain y to be the set of the elements in x */
predicate fzn_array_int_union(array[int] of var int: x, var set of int: y) =
    y = array_union([let { var set of dom(x[i]): s;
                           constraint x[i] in s /\ card(s)=1;
                         } in s | i in index_set(x)]);
