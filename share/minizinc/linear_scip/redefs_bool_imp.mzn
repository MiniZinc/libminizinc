include "redefs_lin_halfreifs.mzn";

% SIMPLE BOOLEAN LOGIC
% TODO: why not check "is_fixed(r)" everywhere??

predicate bool_eq_imp(var bool: p, var bool: q, var bool: r) =
  if is_fixed(r) then
    if fix(r) then p = q else true endif
  else
    let {
      var int: x = bool2int(p),
      var int: y = bool2int(q),
      var int: z = bool2int(r)
    } in x + z <= y + 1 /\ y + z <= x + 1
  endif;

predicate bool_ne_imp(var bool: p, var bool: q, var bool: r) =
  bool_xor_imp(p, q, r);

predicate bool_le_imp(var bool: p, var bool: q, var bool: r) =
  let {
    var int: x = bool2int(p),
    var int: y = bool2int(q),
    var int: z = bool2int(r),
  } in 1 - x + y >= z;

predicate bool_lt_imp(var bool: p, var bool: q, var bool: r) =
  bool_and_imp(not p, q, r);

predicate bool_or_imp(var bool: p, var bool: q, var bool: r) =
  array_bool_or_imp([p,q], r);

predicate bool_and_imp(var bool: p, var bool: q, var bool: r) =
  array_bool_and_imp([p,q],r);

predicate bool_xor_imp(var bool: p, var bool: q, var bool: r) =
  let {
    var int: x = bool2int(p),
    var int: y = bool2int(q),
    var int: z = bool2int(r),
  } in x + y >= z /\ x + y + z <= 2;

% BOOLEAN ARRAY OPERATIONS
predicate array_bool_or_imp(array[int] of var bool: a, var bool: b) =
  if forall( i in index_set( a ) )( is_fixed(a[i]) /\ not fix(a[i]) ) then
    not b
  elseif exists( i in index_set( a ) )( is_fixed(a[i]) /\ fix(a[i]) ) then
    true
  else
    let {
      array[index_set(a)] of var bool: a1;
      var int: x = bool2int(b),
    } in forall(i in index_set(a)) (a1[i] -> a[i]) /\ sum(a1) = x
  endif;

predicate array_bool_and_imp(array[int] of var bool: a, var bool: b) =
  if is_fixed(b) then
    if fix(b) then
      forall(i in index_set(a))( a[i] )
    else
      true
    endif
  elseif forall( i in index_set( a ) )( is_fixed(a[i]) /\ fix(a[i]) ) then
    true
  else
    let {
      var int: x = bool2int(b),
      array[index_set(a)] of var int: c =
        array1d(index_set(a), [ bool2int(a[i] ) | i in index_set(a) ])
    } in forall(i in index_set(c)) ( c[i] >= x )
  endif;

%% No var int d, sorry TODO: why not???
predicate bool_lin_eq_imp(array[int] of int: c, array[int] of var bool: x,
                          int: d, var bool: b) =
  aux_int_eq_if_1(sum(i in index_set(x))( c[i]*bool2int(x[i]) ), d, bool2int(b));
