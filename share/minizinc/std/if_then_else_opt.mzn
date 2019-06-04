predicate if_then_else(array[int] of var bool: c, array[int] of opt int: x, var opt int: y) =
  if_then_else(array1d(c),[deopt(x[i]) | i in index_set(x)],deopt(y)) /\
  if_then_else(array1d(c),[occurs(x[i]) | i in index_set(x)],occurs(y));
predicate if_then_else(array[int] of var bool: c, array[int] of var opt int: x, var opt int: y) =
  if_then_else(array1d(c),[deopt(x[i]) | i in index_set(x)],deopt(y)) /\
  if_then_else(array1d(c),[occurs(x[i]) | i in index_set(x)],occurs(y));
predicate if_then_else(array[int] of var bool: c, array[int] of opt bool: x, var opt bool: y) =
  if_then_else(array1d(c),[deopt(x[i]) | i in index_set(x)],deopt(y)) /\
  if_then_else(array1d(c),[occurs(x[i]) | i in index_set(x)],occurs(y));
predicate if_then_else(array[int] of var bool: c, array[int] of var opt bool: x, var opt bool: y) =
  if_then_else(array1d(c),[deopt(x[i]) | i in index_set(x)],deopt(y)) /\
  if_then_else(array1d(c),[occurs(x[i]) | i in index_set(x)],occurs(y));
