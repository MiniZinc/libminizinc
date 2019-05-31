predicate fzn_maximum_arg_float(array[int] of var float: x, var int: i) =
    let { int: l = min(index_set(x));
          int: u = max(index_set(x));
          float: ly = lb_array(x);
          float: uy = ub_array(x);
          array[l..u] of var ly..uy: y; 
          array[l..u] of var l..u: mi; } in
    y[l] = x[l] /\ mi[l] = l /\ i = mi[u] /\
    forall (j in l+1 .. u) ( y[j] == max(x[j],y[j-1]) /\
                             mi[j] = if y[j-1] >= x[j] then mi[j-1] else j endif );
