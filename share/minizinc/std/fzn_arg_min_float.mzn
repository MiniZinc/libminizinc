predicate fzn_minimum_arg_float(array[int] of var float: x, var int: i) =
    let { int: l = min(index_set(x));
          int: u = max(index_set(x));
          float: ly = lb_array(x);
        } in
    if exists(j in l..u)(ub(x[j]) = ly)
    then let { array[l..u] of var bool: d; } in
      % min is known to be ly
      x[i] = ly /\                   % ith case must be equal to ub
      forall(j in l..u)(x[j] = ly -> i <= j) /\ % lower bound
      d[l] = (x[l] != ly) /\
      forall(j in l+1..u)(d[j] <-> (d[j-1] /\ (x[j] != ly))) /\
      forall(j in l..u)(d[j] -> i >= j+1)    % upper bound
    else
    let { float: uy = ub_array(x);
          array[l..u] of var ly..uy: y; 
          array[l..u] of var l..u: mi; } in
    y[l] = x[l] /\ mi[l] = l /\ i = mi[u] /\
    forall (j in l+1 .. u) ( y[j] == min(x[j],y[j-1]) /\
                             mi[j] = if y[j-1] <= x[j] then mi[j-1] else j endif )
    endif;
