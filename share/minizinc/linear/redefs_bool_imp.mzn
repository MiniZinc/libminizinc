include "redefs_lin_halfreifs.mzn";

% SIMPLE BOOLEAN LOGIC
% TODO: why not check "is_fixed(r)" everywhere??

predicate bool_eq_imp(var bool: x, var bool: y, var bool: b :: promise_ctx_antitone) =
  if is_fixed(b) then
    if fix(b) then x = y else true endif
  else
    let {
      var int: xi = bool2int(x);
      var int: yi = bool2int(y);
      var int: zi = bool2int(b);
    } in xi + zi <= yi + 1 /\ yi + zi <= xi + 1
  endif;

predicate bool_ne_imp(var bool: x, var bool: y, var bool: b :: promise_ctx_antitone) =
  bool_xor_imp(x, y, b);

predicate bool_le_imp(
  var bool: x :: promise_ctx_antitone,
  var bool: y :: promise_ctx_monotone,
  var bool: b :: promise_ctx_antitone,
) =
  let {
    var int: xi = bool2int(x);
    var int: yi = bool2int(y);
    var int: zi = bool2int(b);
  } in 1 - xi + yi >= zi;

predicate bool_lt_imp(
  var bool: x :: promise_ctx_antitone,
  var bool: y :: promise_ctx_monotone,
  var bool: b :: promise_ctx_antitone,
) = bool_and_imp(not x, y, b);

predicate bool_or_imp(
  var bool: x :: promise_ctx_monotone,
  var bool: y :: promise_ctx_monotone,
  var bool: b :: promise_ctx_antitone,
) = array_bool_or_imp([x, y], b);

predicate bool_and_imp(
  var bool: x :: promise_ctx_monotone,
  var bool: y :: promise_ctx_monotone,
  var bool: b :: promise_ctx_antitone,
) = array_bool_and_imp([x, y], b);

predicate bool_xor_imp(var bool: x, var bool: y, var bool: b :: promise_ctx_antitone) =
  let {
    var int: xi = bool2int(x);
    var int: yi = bool2int(y);
    var int: zi = bool2int(b);
  } in xi + yi >= zi /\ xi + yi + zi <= 2;

% BOOLEAN ARRAY OPERATIONS
predicate array_bool_or_imp(
  array [int] of var bool: xs :: promise_ctx_monotone,
  var bool: b :: promise_ctx_antitone,
) =
  if forall (i in index_set(xs)) (is_fixed(xs[i]) /\ not fix(xs[i])) then
    not b
  elseif exists (i in index_set(xs)) (is_fixed(xs[i]) /\ fix(xs[i])) then
    true
  else
    let {
      array [index_set(xs)] of var bool: a1;
      var int: x = bool2int(b);
    } in forall (i in index_set(xs)) (a1[i] -> xs[i]) /\ sum(a1) = x
  endif;

predicate array_bool_and_imp(
  array [int] of var bool: xs :: promise_ctx_monotone,
  var bool: b :: promise_ctx_antitone,
) =
  if is_fixed(b) then
    if fix(b) then forall (i in index_set(xs)) (xs[i]) else true endif
  elseif forall (i in index_set(xs)) (is_fixed(xs[i]) /\ fix(xs[i])) then
    true
  else
    let {
      var int: x = bool2int(b);
      array [index_set(xs)] of var int: c =
        array1d(index_set(xs), [bool2int(xs[i]) | i in index_set(xs)]);
    } in forall (i in index_set(c)) (c[i] >= x)
  endif;

%% No var int d, sorry TODO: why not???
predicate bool_lin_eq_imp(
  array [int] of int: as,
  array [int] of var bool: xs,
  int: y,
  var bool: b :: promise_ctx_antitone,
) = aux_int_eq_if_1(sum (i in index_set(xs)) (as[i] * bool2int(xs[i])), y, bool2int(b));
