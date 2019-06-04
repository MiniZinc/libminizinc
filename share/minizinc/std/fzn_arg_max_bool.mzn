predicate fzn_maximum_arg_bool(array[int] of var bool: x, var int: i) =
    let { int: l = min(index_set(x));
          int: u = max(index_set(x));
          array[l..u] of var bool: y; 
          array[l..u] of var l..u: mi; } in
    y[l] = x[l] /\ mi[l] = l /\ i = mi[u] /\
    forall (j in l+1 .. u) ( y[j] == (x[j] \/ y[j-1]) /\
                             mi[j] = if (y[j-1] \/ not x[j]) then mi[j-1] else j endif );