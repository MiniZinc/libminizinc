annotation is_defined_var;
annotation var_is_introduced;
annotation defines_var(var $t: c);
annotation output_array(array[$u] of set of int:a);
annotation output_var;
annotation domain;


annotation complete;
annotation bounds;

annotation int_search(array[int] of var $t:a, ann:b, ann:c, ann:d);
annotation bool_search(array[int] of var $t:a, ann:b, ann:c, ann:d);
annotation seq_search(array[int] of ann:a);


%Variable choice
annotation input_order;
annotation smallest;
annotation largest;
annotation first_fail;
annotation anti_first_fail;

%Value choice
annotation indomain;
annotation indomain_min;
annotation indomain_max;
annotation indomain_split;
annotation indomain_reverse_split;
annotation indomain_middle;
annotation indomain_median;
annotation indomain_random;
annotation indomain_interval;

%Predicate definitions for stdlib.fzn

%int constraints

predicate int_abs(var int: a, var int: b);
predicate int_eq(var int: a, var int: b);
predicate int_eq_reif(var int: a, var int: b, var bool: r);
predicate int_le(var int: a, var int: b);
predicate int_le_reif(var int: a, var int: b, var bool: r);
predicate int_lin_eq(array[int] of int: as, array[int] of var int: bs, int: c);
predicate int_lin_eq_reif(array[int] of int: as, array[int] of var int: bs,int: c, var bool: r);
predicate int_lin_ne(array[int] of int: as, array[int] of var int: bs, int: c);
predicate int_lin_ne_reif(array[int] of int: as, array[int] of var int: bs,int: c, var bool: r);

predicate int_lin_le(array[int] of int: as, array[int] of var int: bs, int: c);
predicate int_lin_le_reif(array[int] of int: as, array[int] of var int: bs,int: c, var bool: r);
predicate int_ne(var int: a, var int: b);
predicate int_ne_reif(var int: a, var int: b, var bool: r);
predicate int_plus(var int: a, var int: b, var int: c);
predicate int_times_le(var int: a, var int: b, var int: c);
predicate int_div(var int: a, var int: b, var int: c);
predicate int_lt(var int: a, var int: b);
predicate int_lt_reif(var int: a, var int: b, var bool: r);
predicate int_max(var int: a, var int: b, var int: c);
predicate int_min(var int: a, var int: b, var int: c);
predicate int_mod(var int: a, var int: b, var int: c);
predicate int_times(var int: a, var int: b, var int: c);



%Array constraints
predicate array_bool_and(array[int] of var bool: as, var bool: r);
predicate array_bool_or(array[int] of var bool: as, var bool: r);
predicate array_bool_element(var int: b, array[int] of bool: as, var bool: c);
predicate array_int_element(var int: b, array[int] of int: as, var int: c);
predicate array_var_bool_element(var int: b, array[int] of var bool: as, var bool: c);
predicate array_var_int_element(var int: b, array[int] of var int: as, var int: c);

%Boolean constraints
predicate bool2int(var bool: a, var int: b);
predicate bool_and(var bool: a, var bool: b, var bool: r);
predicate bool_clause(array[int] of var bool: as, array[int] of var bool: bs);
predicate bool_eq(var bool: a, var bool: b);
predicate bool_eq_reif(var bool: a, var bool: b, var bool: r);
predicate bool_le(var bool: a, var bool: b);
predicate bool_le_reif(var bool: a, var bool: b, var bool: r);
predicate bool_lin_eq(array[int] of int: as, array[int] of var bool: bs, var int: c);
predicate bool_lin_le(array[int] of int: as, array[int] of var bool: bs, int: c);
predicate bool_lt(var bool: a, var bool: b);
predicate bool_lt_reif(var bool: a, var bool: b, var bool: r);
predicate bool_not(var bool: a, var bool: b);
predicate bool_or(var bool: a, var bool: b, var bool: r);
predicate bool_xor(var bool: a, var bool: b, var bool: r);


