predicate gecode_minimum_arg_int(array[int] of var int: x, var int: i);

predicate fzn_minimum_arg_int(array[int] of var int: x, var int: i) =
  gecode_minimum_arg_int(x,(i-min(index_set(x)))::domain);
