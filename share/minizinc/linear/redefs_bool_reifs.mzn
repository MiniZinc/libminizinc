/*
% FlatZinc built-in redefinitions for linear solvers.
%
% AUTHORS
% Sebastian Brand
% Gleb Belov
*/

%-----------------------------------------------------------------------------%
%
% Logic operations
% Use indicators for reifs (CPLEX)?  Seems weak.
%
%-----------------------------------------------------------------------------%

predicate bool_not(var bool: p) = bool2int(p) = 0;

predicate bool_not(var bool: x, var bool: y) = bool2int(x) + bool2int(y) = 1;

predicate bool_and(var bool: x, var bool: y, var bool: b) =
  %  my_trace("  bool_and: \(x) /\\ \(y) <-> \(b) \n") /\
  if false then
    int_lin_le_reif__IND([-1, -1], [x, y], -2, b)
  else
    array_bool_and([x, y], b)
    %    bool_and__INT(bool2int(x), bool2int(y), bool2int(b))
  endif;

predicate bool_and__INT(var int: x, var int: y, var int: z) =
  x + y <= z + 1 /\
    %% x + y >= z * 2;         % weak
    x >= z /\
    y >= z; % strong

predicate bool_or(var bool: x, var bool: y, var bool: b) =
  if false then
    int_lin_le_reif__IND([-1, -1], [x, y], -1, b)
  elseif true then
    array_bool_or([x, y], b)
  else
    let {
      var int: xi = bool2int(x);
      var int: yi = bool2int(y);
      var int: zi = bool2int(b);
    } in xi + yi >= zi /\
      % x + y <= z * 2;       % weak
      xi <= zi /\
      yi <= zi % strong
  endif;

predicate bool_xor(var bool: x, var bool: y) = 1 == x + y;

predicate bool_xor(var bool: x, var bool: y, var bool: b) =
  if false then
    %      int_lin_eq_reif__IND( [1, 1], [x, y], 1, b) /\
    true
  else
    let {
      var int: xi = bool2int(x);
      var int: yi = bool2int(y);
      var int: zi = bool2int(b);
    } in xi <= yi + zi /\ yi <= xi + zi /\ zi <= xi + yi /\ xi + yi + zi <= 2
  endif;

predicate bool_eq_reif(var bool: x, var bool: y, var bool: b) =
  %%    trace(" bool_eq_reif: \(x), \(y), \(b) \n") /\
  if is_fixed(b) then % frequent case
    if fix(b) = true then x = y else bool_not(x, y) endif
  elseif is_fixed(y) then
    if fix(y) = true then x = b else bool_not(x, b) endif
  elseif is_fixed(x) then
    if fix(x) = true then y = b else bool_not(y, b) endif
  elseif false then
    %      int_lin_eq_reif__IND( [1, -1], [x, y], 0, b) /\
    true
  else
    let {
      var int: xi = bool2int(x);
      var int: yi = bool2int(y);
      var int: zi = bool2int(b);
    } in xi + yi <= zi + 1 /\ xi + zi <= yi + 1 /\ yi + zi <= xi + 1 /\ xi + yi + zi >= 1
  endif;

predicate bool_ne_reif(var bool: x, var bool: y, var bool: b) = bool_xor(x, y, b);

predicate bool_le(var bool: x :: promise_ctx_antitone, var bool: y :: promise_ctx_monotone) =
  let {
    var int: xi :: promise_ctx_antitone = bool2int(x);
    var int: yi :: promise_ctx_monotone = bool2int(y);
  } in xi <= yi;

predicate bool_le_reif(var bool: x, var bool: y, var bool: b) =
  if false then
    %      int_lin_le_reif__IND( [1, -1], [x, y], 0, b) /\
    true
  else
    let {
      var int: xi = bool2int(x);
      var int: yi = bool2int(y);
      var int: zi = bool2int(b);
    } in 1 - xi + yi >= zi /\
      %% /\ 1 - x + y <= z * 2      not needed
      1 - xi <= zi /\
      yi <= zi % strong
  endif;

predicate bool_lt(var bool: x :: promise_ctx_antitone, var bool: y :: promise_ctx_monotone) =
  not x /\ y;

predicate bool_lt_reif(var bool: x, var bool: y, var bool: b) = (not x /\ y) <-> b;

%-----------------------------------------------------------------------------%

%% Reified disjunction
predicate array_bool_or(array [int] of var bool: xs, var bool: b) =
  if exists (i in index_set(xs)) (is_fixed(xs[i]) /\ fix(xs[i])) then
    b
  elseif is_fixed(b) then % frequent case
    if fix(b) = true then
      sum (i in index_set(xs)) (bool2int(xs[i])) >= 1 %% >=1 seems better for MIPDomains... 5.4.19
    else
      forall (i in index_set(xs)) (not xs[i])
    endif
  else
    let {
      var int: x = bool2int(b);
      array [1..length(xs)] of var int: c = [bool2int(xs[i]) | i in index_set(xs)];
    } in sum(c) >= x /\
      %  sum(c) <= x * length(a)                     % weak
      forall (i in index_set(xs)) (x >= c[i]) % strong
  endif;

%% Reified conjunction
predicate array_bool_and(array [int] of var bool: xs, var bool: b) =
  if exists (i in index_set(xs)) (is_fixed(xs[i]) /\ not fix(xs[i])) then
    not b
  elseif is_fixed(b) then % frequent case
    if fix(b) = false then
      sum (i in index_set(xs)) (bool2int(xs[i])) <= length(xs) - 1
    else
      forall (i in index_set(xs)) (xs[i])
    endif
  else
    let {
      var int: x = bool2int(b);
      array [1..length(xs)] of var int: c = [bool2int(xs[i]) | i in index_set(xs)];
    } in length(xs) - sum(c) >= 1 - x /\
      % length(a) - sum(c) <= (1 - x) * length(a);     % weak
      forall (i in index_set(xs)) (x <= c[i]) % strong
  endif;

% predicate array_bool_xor(array[int] of var bool: xs) = .. sum(xs) is odd ..
predicate array_bool_xor(
  array [int] of var bool: xs,
) =
  let {
    var 0..(length(xs) - 1) div 2: m;
    var 1..((length(xs) - 1) div 2) * 2 + 1: ss = sum (i in index_set(xs)) (bool2int(xs[i]));
  } in ss == 1 + 2 * m;

predicate bool_clause(
  array [int] of var bool: xs :: promise_ctx_monotone,
  array [int] of var bool: ys :: promise_ctx_antitone,
) =
  sum (i in index_set(xs)) (bool2int(xs[i])) -
    sum (i in index_set(ys)) (bool2int(ys[i])) +
     length(ys) >=
    1;

predicate bool_lin_eq(array [int] of int: as, array [int] of var bool: xs, var int: y)
  :: promise_total = sum (i in index_set(xs)) (as[i] * bool2int(xs[i])) == y;

predicate bool_lin_eq_reif(
  array [int] of int: as,
  array [int] of var bool: xs,
  int: y,
  var bool: b, %% No var int d, sorry
) = aux_int_eq_iff_1(sum (i in index_set(xs)) (as[i] * bool2int(xs[i])), y, bool2int(b));
