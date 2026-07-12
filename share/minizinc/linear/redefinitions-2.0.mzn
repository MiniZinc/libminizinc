predicate bool_clause_reif(array [int] of var bool: xs, array [int] of var bool: ys, var bool: b) =
  b =
    (
      sum (i in index_set(xs)) (bool2int(xs[i])) -
        sum (i in index_set(ys)) (bool2int(ys[i])) +
        length(ys) >=
        1
    );

predicate array_int_minimum(var int: m, array [int] of var int: xs) =
  array_int_minimum_I(m, [xs[i] | i in index_set(xs)]);
predicate array_int_maximum(var int: m, array [int] of var int: xs) =
  array_int_minimum_I(-m, [-xs[i] | i in index_set(xs)]);
predicate array_float_minimum(var float: m, array [int] of var float: xs) =
  array_float_minimum_I(m, [xs[i] | i in index_set(xs)]);
predicate array_float_maximum(var float: m, array [int] of var float: xs) =
  array_float_minimum_I(-m, [-xs[i] | i in index_set(xs)]);
