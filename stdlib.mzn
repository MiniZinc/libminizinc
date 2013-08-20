annotation total;

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

% Element constraints

predicate array_int_element(var int, array[int] of var int, var int);

function var int: element_t(var int: idx, array[int] of var int: x) ::total =
  let {
    var int: r;
    constraint array_int_element(idx-(min(index_set(x))-1),[x[i] | i in index_set(x)],r);
  } in r;

function var int: element_mt(var int: idx, array[int] of var int: x) ::total =
  let {
    var int: r;
    var int: idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_int_element(idx2-(min(index_set(x))-1),[x[i] | i in index_set(x)],r);
  } in r;

function var int: element(var int: idx, array[int] of var int: x) =
  if (lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var int: element(var int: idx1, int: idx2,
                          array[int,int] of var int: x) =
  element(idx1,
          array1d(min(index_set_1of2(x))..max(index_set_1of2(x)),
                  [x[i,idx2] | i in index_set_1of2(x)]));

function var int: element(int: idx1, var int: idx2,
                          array[int,int] of var int: x) =
  element(idx2,
          array1d(min(index_set_2of2(x))..max(index_set_2of2(x)),
                  [x[idx1,i] | i in index_set_2of2(x)]));
