predicate fzn_nvalue_reif(var int: n, array[int] of var int: x, var bool: b) =
    let { int: lx = lb_array(x),
          int: ux = ub_array(x),
    } in b <->
        n == sum(j in lx..ux) (
            bool2int(exists(i in index_set(x)) ( x[i] = j ))
        );
