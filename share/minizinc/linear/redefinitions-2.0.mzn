predicate bool_clause_reif(array[int] of var bool: p, array[int] of var bool: n, var bool: c) =
    c = ( sum(i in index_set(p))( bool2int(p[i]) )
          - sum(i in index_set(n))( bool2int(n[i]) )
          + length(n)
            >= 1
        );

predicate array_int_minimum(var int: m, array[int] of var int: x) =
    array_int_minimum_I( m, [ x[i] | i in index_set(x)]);
predicate array_int_maximum(var int: m, array[int] of var int: x) =
    array_int_minimum_I(-m, [-x[i] | i in index_set(x)]);
predicate array_float_minimum(var float: m, array[int] of var float: x) =
    array_float_minimum_I( m, [ x[i] | i in index_set(x)]);
predicate array_float_maximum(var float: m, array[int] of var float: x) =
    array_float_minimum_I(-m, [-x[i] | i in index_set(x)]);

