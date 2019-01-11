include "redefs_lin_halfreifs.mzn";
include "redefs_lin_reifs.mzn";

%% var, var
predicate int_le_imp(var int: x, var int: y, var bool: b) =
  if is_fixed(b) then
    if fix(b) then x<=y else true endif
  elseif ub(x)<=lb(y) then true
  elseif lb(x)>ub(y) then (not b)
  % elseif fPostprocessDomains /\ fPostproDom_DIFF then
  %   % TODO MIPDomains
  else
    aux_int_le_if_1(x, y, b)
  endif;

%% var, var
predicate int_lt_imp(var int: x, var int: y, var bool: b) =
  if is_fixed(x) then
    int_le_imp(x + 1, y, b)
  else
    int_le_imp(x, y - 1, b)
  endif;

%% var, var
predicate int_eq_imp(var int: x, var int: y, var bool: b) =
  if is_fixed(b) then
    if fix(b) then (x = y) else true endif
  elseif card( dom(x) intersect dom(y) ) > 0 then
    if is_fixed(x) then
      if is_fixed(y) then
        b -> (fix(x) = fix(y))
      else
        int_eq_imp(y, fix(x), b)
      endif
    elseif is_fixed(y) then
      int_eq_imp(x, fix(y), b)
    % elseif fPostprocessDomains /\ fPostproDom_DIFF then
    % % TODO MIPDomains
    else
      aux_int_eq_if_1(x, y, b)
    endif
  else
    not b
  endif;

%% var, const
predicate int_eq_imp(var int: x, int: y, var bool: b) =
  if is_fixed(b) then
    if fix(b) then (x = y) else true endif
  elseif y in dom(x) then
    if is_fixed(x) then
      b -> (y = fix(x))
    % elseif fPostprocessDomains then
    % % TODO MIPDomains
    elseif card(dom(x)) < nMZN__UnaryLenMax_eq then % TODO: Use eq_encode in imp ??
      let {
        array[int] of var int: p = eq_encode(x);
      } in b <= p[y]
    else
      aux_int_eq_if_1(x, y, b)
    endif
  else
    not b
  endif;

%% var, var
predicate int_ne_imp(var int: x, var int: y, var bool: b) =
  if is_fixed(b) then
    if fix(b) then (x != y) else true endif
  elseif (ub(x) < lb(y)) \/ (lb(x) > ub(y)) then
    true
  else
    if is_fixed(x) then
      if is_fixed(y) then
        b -> (fix(x) != fix(y))
      else
        int_ne_imp(y, fix(x), b)
      endif
    elseif is_fixed(y) then
      int_ne_imp(x, fix(y), b)
    % elseif fPostprocessDomains /\ fPostproDom_DIFF then
    % % TODO MIPDomains
    else
      aux_int_ne_if_1(x, y, b)
    endif
  endif;

%% var, const
predicate int_ne_imp(var int: x, int: y, var bool: b) =
  if is_fixed(b) then
    if fix(b) then (x != y) else true endif
  elseif (ub(x) < lb(y)) \/ (lb(x) > ub(y)) then
    true
  else
    if is_fixed(x) then
      b -> (y != fix(x))
    % elseif fPostprocessDomains then
    % % TODO MIPDomains
    elseif card(dom(x)) < nMZN__UnaryLenMax_eq then
      let {
        array[int] of var int: p = eq_encode(x);
      } in b <= (1 - p[y])
    else
      aux_int_ne_if_1(x, y, b)
    endif
  endif;

%-----------------------------------------------------------------------------%

%% lin_expr, const
predicate int_lin_le_imp(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
  if (d = 0) /\ (length(c) = 2) /\ (abs(c[1]) = 1) /\ (c[1] = -1 * c[2]) then
    if (c[1] < 0) then
      int_le_imp(x[2], x[1], b)
    else
      int_le_imp(x[1], x[2], b)
    endif
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    int_le_imp(sum(i in index_set(x))(c[i] * x[i]), d, b)
  elseif fAvoidNI then
    aux_float_le_if_1(sum2float(c, x), d, b)
  else
    aux_int_le_if_1(sum(i in index_set(x))(c[i] * x[i]), d, b)
  endif;

predicate int_lin_lt_imp(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
  if true then
    abort("int_lin_lt_imp not supposed to be called")
  else
    int_lin_le_imp(c, x, d - 1, b)
  endif;

%% lin_expr, const
predicate int_lin_eq_imp(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
  if (d = 0) /\ (length(c) = 2) /\ (abs(c[1]) = 1) /\ (c[1] = -1 * c[2]) then
    int_ne_imp(x[1], x[2], b)
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    int_eq_imp(sum(i in index_set(x))(c[i] * x[i]), d, b)
  elseif fAvoidNI then
    aux_float_eq_if_1(sum2float(c, x), d, b)
  else
    aux_int_eq_if_1(sum(i in index_set(x))(c[i] * x[i]), d, b)
  endif;

%% lin_expr, const
predicate int_lin_ne_imp(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
  if (d = 0) /\ (length(c) = 2) /\ (abs(c[1]) = 1) /\ (c[1] = -1 * c[2]) then
      int_ne_imp(x[1], x[2], b)
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    int_ne_imp(sum(i in index_set(x))(c[i] * x[i]), d, b)
  elseif fAvoidNI then
    aux_float_ne_if_1(sum2float(c, x), d, b)
  else
    aux_int_ne_if_1(sum(i in index_set(x))(c[i] * x[i]), d, b)
  endif;

%-----------------------------------------------------------------------------%

%% var float, var float
predicate float_le_imp(var float: x, var float: y, var bool: b) =
  if is_fixed(b) then
    if fix(b) then x <= y else true endif
  elseif ub(x) <= lb(y) then true
  elseif lb(x) > ub(y) then (not b)
  % elseif fPostprocessDomains /\ fPostproDom_DIFF then
  % % TODO MIPDomains
  else
    aux_float_le_if_1(x, y, b)
  endif;

%% var float, var float
predicate float_lt_imp(var float: x, var float: y, var bool: b) =
  if is_fixed(b) then
    if fix(b) then (x < y) else true endif
  % elseif fPostprocessDomains /\ fPostproDom_DIFF then
  % % TODO MIPDomains
  else
    aux_float_lt_if_1(x, y, b)
  endif;

%% var float, var float
predicate float_eq_imp(var float: x, var float: y, var bool: b) =
  if is_fixed(b) then
    if fix(b) then (x = y) else true endif
  elseif (ub(x) < lb(y)) \/ (lb(x) > ub(y)) then
    not b
  elseif is_fixed(x) /\ is_fixed(y) then
    b -> (fix(x) == fix(y))
  % elseif fPostprocessDomains /\ fPostproDom_DIFF then
  % % TODO MIPDomains
  else
    aux_float_eq_if_1(x, y, b)
  endif;

%% var float, var float
predicate float_ne_imp(var float: x, var float: y, var bool: b) =
  if is_fixed(b) then
    if fix(b) then (x != y) else true endif
  elseif (ub(x) < lb(y)) \/ (lb(x) > ub(y)) then
    true
  elseif is_fixed(x) /\ is_fixed(y) then
    b -> (fix(x) != fix(y))
  % elseif fPostprocessDomains /\ fPostproDom_DIFF then
  % % TODO MIPDomains
  else
    aux_float_ne_if_1(x, y, b)
  endif;

%-----------------------------------------------------------------------------%

predicate float_lin_eq_imp(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
  if (d = 0.0) /\ (length(c) = 2) /\ (abs(c[1]) = 1.0) /\ (c[1] = -1.0 * c[2]) then
    float_eq_imp(x[1], x[2], b)
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    float_eq_imp(sum(i in index_set(x))(c[i] * x[i]), d, b)
  else
    aux_float_eq_if_1(sum(i in index_set(x))(c[i] * x[i]), d, b)
  endif;

predicate float_lin_ne_imp(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
  if (d = 0.0) /\ (length(c) = 2) /\ (abs(c[1]) = 1.0) /\ (c[1] = -1.0 * c[2]) then
    float_ne_imp(x[1], x[2], b)
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    float_ne_imp(sum(i in index_set(x))(c[i] * x[i]), d, not b)
  else
    aux_float_ne_if_1(sum(i in index_set(x))(c[i] * x[i]), d, b)
  endif;

predicate float_lin_le_imp(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
  if (d = 0.0) /\ (length(c) = 2) /\ (abs(c[1]) = 1.0) /\ (c[1] = -1.0 * c[2]) then
    if (c[1] < 0.0) then
      float_le_imp(x[2], x[1], b)
    else
      float_le_imp(x[1], x[2], b)
    endif
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    float_le_imp(sum(i in index_set(x))(c[i] * x[i]), d, b)
  else
    aux_float_le_if_1(sum(i in index_set(x))(c[i] * x[i]), d, b)
  endif;

predicate float_lin_lt_imp(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
  if (d = 0.0) /\ (length(c) = 2) /\ (abs(c[1]) = 1.0) /\ (c[1] = -1.0 * c[2]) then
    if (c[1] < 0.0) then
      float_lt_imp(x[2], x[1], b)
    else
      float_lt_imp(x[1], x[2], b)
    endif
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    float_lt_imp(sum(i in index_set(x))(c[i] * x[i]), d, b)
  else
    aux_float_lt_if_1(sum(i in index_set(x))(c[i] * x[i]), d, b)
  endif;
