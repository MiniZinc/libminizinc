annotation is_defined_var;
annotation var_is_introduced;
annotation defines_var(var int: c);
annotation output_array(array[int] of set of int:a);

%Predicate definitions for stdlib.fzn
predicate int2float(var int:a, var float:b);
predicate int_abs(var int: a, var int: b);
predicate int_eq(var int: a, var int: b);
predicate int_eq_reif(var int: a, var int: b, var bool: r);
predicate int_le(var int: a, var int: b);
predicate int_le_reif(var int: a, var int: b, var bool: r);
predicate int_lin_eq(array [int] of int: as, array [int] of var int: bs, int: c);
predicate int_lin_eq_reif(array [int] of int: as, array [int] of var int: bs,int: c, var bool: r);
predicate int_lin_le(array [int] of int: as, array [int] of var int: bs, int: c);
predicate int_lin_le_reif(array [int] of int: as, array [int] of var int: bs,int: c, var bool: r);

predicate int_ne(var int: a, var int: b);
predicate int_ne_reif(var int: a, var int: b, var bool: r);
predicate int_plus(var int: a, var int: b, var int: c);
predicate int_times(var int: a, var int: b, var int: c);

predicate array_bool_and(array [int] of var bool: as, var bool: r);

predicate array_bool_or(array [int] of var bool: as, var bool: r);
predicate array_bool_xor(array [int] of var bool: as);








predicate bool2int(var bool: a, var int: b);
predicate bool_and(var bool: a, var bool: b, var bool: r);
predicate bool_clause(array [int] of var bool: as, array [int] of var bool: bs);
predicate bool_eq(var bool: a, var bool: b);
predicate bool_eq_reif(var bool: a, var bool: b, var bool: r);
predicate bool_le(var bool: a, var bool: b);
predicate bool_le_reif(var bool: a, var bool: b, var bool: r);
predicate bool_lin_eq(array [int] of int: as, array [int] of var bool: bs, var int: c);
predicate bool_lin_le(array [int] of int: as, array [int] of var bool: bs, int: c);
predicate bool_lt(var bool: a, var bool: b);
predicate bool_lt_reif(var bool: a, var bool: b, var bool: r);
predicate bool_not(var bool: a, var bool: b);
predicate bool_or(var bool: a, var bool: b, var bool: r);
predicate bool_xor(var bool: a, var bool: b, var bool: r);

predicate float_abs(var float: a, var float: b);
predicate float_acos(var float: a, var float: b);
predicate float_asin(var float: a, var float: b);
predicate float_atan(var float: a, var float: b);
predicate float_cos(var float: a, var float: b);
predicate float_cosh(var float: a, var float: b);
predicate float_exp(var float: a, var float: b);
predicate float_ln(var float: a, var float: b);
predicate float_log10(var float: a, var float: b);
predicate float_log2(var float: a, var float: b);
predicate float_sqrt(var float: a, var float: b);
predicate float_sin(var float: a, var float: b);
predicate float_sinh(var float: a, var float: b);
predicate float_tan(var float: a, var float: b);
predicate float_tanh(var float: a, var float: b);
predicate float_eq(var float: a, var float: b);
predicate float_eq_reif(var float: a, var float: b, var bool: r);
predicate float_le(var float: a, var float: b);
predicate float_le_reif(var float: a, var float: b, var bool: r);
predicate float_lin_eq(array [int] of float: as, array [int] of var float: bs, float: c);
predicate float_lin_eq_reif(array [int] of float: as, array [int] of var float: bs, float: c, var bool: r);
predicate float_lin_le(array [int] of float: as, array [int] of var float: bs, float: c);
predicate float_lin_le_reif(array [int] of float: as, array [int] of var float: bs, float: c, var bool: r);
predicate float_lt(var float: a, var float: b);
predicate float_lt_reif(var float: a, var float: b, var bool: r);
predicate float_ne(var float: a, var float: b);
predicate float_ne_reif(var float: a, var float: b, var bool: r);
predicate float_plus(var float: a, var float: b, var float: c);
