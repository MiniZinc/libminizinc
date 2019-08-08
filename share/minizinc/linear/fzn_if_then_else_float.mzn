predicate fzn_if_then_else_float(array[int] of var bool: c, array[int] of float: x, var float: y) =
  let { 
    array[index_set(c)] of var 0..1: d;
  } in
  forall (i in index_set(c)) (sum(j in 1..i-1)(c[j])+d[i] >= c[i]) /\
  sum(d)=1 /\
  y = sum (i in index_set(c)) ( d[i]*x[i] );
