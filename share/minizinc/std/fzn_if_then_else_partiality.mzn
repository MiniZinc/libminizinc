predicate fzn_if_then_else_partiality(array[int] of var bool: c, array[int] of var bool: def, var bool: b) =
  let { array[index_set(c)] of var bool: d; } in
  forall(i in index_set(c))
        (if i > min(index_set(c)) then
            d[i] = (not c[i-1] /\ d[i-1])
         else
            d[i] = true
         endif) /\
  forall(i in index_set(c))
        ( (b /\ c[i] /\ d[i] -> def[i])) /\
  forall (i in index_set(c)) (d[i] /\ c[i] -> b);
