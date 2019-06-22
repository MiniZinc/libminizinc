predicate fzn_maximum_arg_bool(array[int] of var bool: x, var int: i) =
    let { int: l = min(index_set(x));
          int: u = max(index_set(x));
        } in
    if exists(j in l..u)(is_fixed(x[j]) /\ fix(x[j]))
    then % special case: max is known to be 1
    let { array[l..u] of var bool: d; } in
         x[i] = true /\                   % ith case must be equal to ub
         forall(j in l..u)(x[j] -> i <= j) /\ % lower bound
         d[l] = x[l] /\
         forall(j in l+1..u)(d[j] <-> (d[j-1] \/ x[j])) /\
         forall(j in l..u)(not d[j] -> i >= j+1)   % upper bound    
    else
      % general case: min could be 0 or 1
    let { array[l..u] of var bool: y; 
          array[l..u] of var l..u: mi; } in
    y[l] = x[l] /\ mi[l] = l /\ i = mi[u] /\
    forall (j in l+1 .. u) ( y[j] == (x[j] \/ y[j-1]) /\
                             mi[j] = if (y[j-1] \/ not x[j]) then mi[j-1] else j endif )
    endif;
