/*
% FlatZinc built-in redefinitions for linear solvers.
%
% AUTHORS
% Sebastian Brand
% Gleb Belov
*/

%-----------------------------------------------------------------------------%
% Auxiliary: indicator constraints
%   p -> x # 0  where p is a 0/1 variable and # is a comparison

% Base cases

%% used e.g. in element
predicate aux_float_eq_if_1(var float: x, var float: y, var int: p) =
  if is_fixed(p) then
    if 1==fix(p) then x==y else true endif
  elseif fPostprocessDomains then
    aux_float_eq_zero_if_1__POST(x - y, p)
  elseif fMZN__UseIndicators then
    my_trace__DBG("  aux_float_eq_if_1  INDIC: \(x) \\/ \(y) <-> \(p) \n") /\
    aux_float_eq_if_1__IND(x, y, p)
  else
    aux_float_le_if_1(x, y, p) /\
    aux_float_ge_if_1(x, y, p)
  endif;

predicate aux_int_le_zero_if_0(var int: x, var int: p) =
%    my_trace("  aux_int_le_zero_if_0( \(x), \(p))\n") /\
%     if card(dom(x))<20 then
%       let { array[int] of var int: pp = eq_encode(x) } in
%       my_trace ("    aux_int_le_zero_if_0 by unary: dom(x) = " ++ show(dom(x)) ++ "\n") /\
%       sum ( i in dom(x) where i<=0 ) ( pp[i] ) == p
%     else
  if is_fixed(p) then
    if 1==fix(p) then x<=0 else true endif
  elseif fPostprocessDomains then
    aux_int_le_zero_if_1__POST(x, 1-p)
  elseif fMZN__UseIndicators then
    my_trace__DBG("  aux_int_le_zero_if_0  INDIC: \(x) \\/ \(p) \n") /\
    aux_int_le_zero_if_0__IND(x, p)
  else
      x <= ub(x) * p
  endif;

predicate aux_float_le_zero_if_0(var float: x, var int: p) =
  if is_fixed(p) then
    if 1==fix(p) then x<=0.0 else true endif
  elseif fPostprocessDomains then
    aux_float_le_zero_if_1__POST(x, 1-p)
  elseif fMZN__UseIndicators then
    my_trace__DBG("  aux_float_le_zero_if_0  INDIC: \(x) \\/ \(p)  \n") /\
    aux_float_le_zero_if_0__IND(x, p)
  else
    x <= ub(x) * p
  endif;

predicate aux_float_lt_zero_if_0(var float: x, var int: p) =
  if is_fixed(p) then
    if 1==fix(p) then x<0.0 else true endif
  elseif fPostprocessDomains then
    aux_float_lt_zero_if_1__POST(x, 1-p, float_lt_EPS_coef__)
  else
    let { float: rho = float_lt_EPS_coef__ * abs(ub(x)) }  % same order of magnitude as ub(x)
    in
    x < (ub(x) + rho) * p
  endif;


% Derived cases

predicate aux_int_le_if_0(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(x - y, p);

predicate aux_int_ge_if_0(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(y - x, p);

predicate aux_int_le_if_1(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(x - y, 1 - p);

predicate aux_int_ge_if_1(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(y - x, 1 - p);

predicate aux_int_lt_if_0(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(x - y + 1, p);

predicate aux_int_gt_if_0(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(y - x + 1, p);

predicate aux_int_lt_if_1(var int: x, int: y, var int: p) =
    aux_int_le_zero_if_0(x - y + 1, 1 - p);


predicate aux_float_le_if_0(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(x - y, p);

predicate aux_float_ge_if_0(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(y - x, p);

predicate aux_float_le_if_1(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(x - y, 1 - p);

predicate aux_float_ge_if_1(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(y - x, 1 - p);

predicate aux_float_lt_if_0(var float: x, var float: y, var int: p) =
    aux_float_lt_zero_if_0(x - y, p);

predicate aux_float_gt_if_0(var float: x, var float: y, var int: p) =
    aux_float_lt_zero_if_0(y - x, p);

predicate aux_float_lt_if_1(var float: x, var float: y, var int: p) =
    aux_float_lt_zero_if_0(x - y, 1 - p);

    
% -------------------------- Domains postpro ---------------------------    
    %% To avoid looking if an original int var x-y exists and has eq_encode:
predicate aux_float_eq_zero_if_1__POST(var float: x, var float: p);

predicate aux_int_le_zero_if_1__POST(var int: x, var int: p);
predicate aux_float_le_zero_if_1__POST(var float: x, var float: p);
predicate aux_float_lt_zero_if_1__POST(var float: x, var float: p, float: eps);

