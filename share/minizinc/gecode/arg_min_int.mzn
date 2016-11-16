predicate gecode_minimum_arg_int(array[int] of var int: x, var int: i);

predicate minimum_arg_int(array[int] of var int: x, var int: i) =
  gecode_minimum_arg_int(x,(i-min(index_set(x)))::domain);
