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
  elseif is_fixed(x) /\ is_fixed(y) then         %%% Needed to avoid constant domain var
    if fix(x)!=fix(y) then p==0 else true endif
  elseif is_fixed(x-y) then                      %%% Hypothetically possible to land here
    if 0.0!=fix(x-y) then p==0 else true endif
  elseif fPostprocessDomains /\ fPostproDom_AUX /\ fPostproDom_DIFF then
    aux_float_eq_zero_if_1__POST(x - y, p, p)
  elseif fMZN__UseIndicators then
    aux_float_eq_if_1__IND(x, y, p)
  else
    aux_float_le_if_1(x, y, p) /\
    aux_float_ge_if_1(x, y, p)
  endif;

predicate aux_int_eq_if_1(var int: x, var int: y, var int: p) =
  if is_fixed(p) then
    if fix(p) = 1 then (x = y) else true endif
  elseif is_fixed(x) /\ is_fixed(y) then
    if fix(x) != fix(y) then (p = 0) else true endif
  % elseif is_fixed(x-y) then % TODO: Necessary for integers??
  %   if 0.0!=fix(x-y) then p==0 else true endif
  % elseif fPostprocessDomains /\ fPostproDom_AUX /\ fPostproDom_DIFF then
  % % TODO MIPDomains
  % elseif fMZN__UseIndicators then % TODO: Necessary for integers??
  %   aux_float_eq_if_1__IND(x, y, p)
  else
    aux_int_le_if_1(x, y, p) /\
    aux_int_ge_if_1(x, y, p)
  endif;

predicate aux_float_ne_if_1(var float: x, var float: y, var int: p) =
  if is_fixed(p) then
    if fix(p) = 1 then (x != y) else true endif
  elseif is_fixed(x) /\ is_fixed(y) then         %%% Needed to avoid constant domain var
    if (fix(x) = fix(y)) then (p = 0) else true endif
  elseif is_fixed(x-y) then                      %%% Hypothetically possible to land here
    if (0.0 = fix(x-y)) then (p = 0) else true endif
  % TODO: What is necessary for not equals?
  % elseif fPostprocessDomains /\ fPostproDom_AUX /\ fPostproDom_DIFF then
  %   aux_float_ne_zero_if_1__POST(x - y, p, p)
  % elseif fMZN__UseIndicators then
  %   aux_float_ne_if_1__IND(x, y, p)
  else
    let { array[1..2] of var bool: q } in
      ( q[1] -> (x < y) ) /\
      ( q[2] -> (x > y) ) /\
      (sum(q) = p)
  endif;

predicate aux_int_ne_if_1(var int: x, var int: y, var int: p) =
  if is_fixed(p) then
    if fix(p) = 1 then (x != y) else true endif
  elseif is_fixed(x) /\ is_fixed(y) then
    if fix(x) = fix(y) then (p = 0) else true endif
  % elseif is_fixed(x-y) then % TODO: Necessary for integers??
  %   if 0.0!=fix(x-y) then p==0 else true endif
  % elseif fPostprocessDomains /\ fPostproDom_AUX /\ fPostproDom_DIFF then
  % % TODO MIPDomains
  % elseif fMZN__UseIndicators then % TODO: Necessary for integers??
  %   aux_float_eq_if_1__IND(x, y, p)
  else
    let { array[1..2] of var bool: q } in
      ( q[1] -> (x < y) ) /\
      ( q[2] -> (x > y) ) /\
      (sum(q) = p)
  endif;

predicate aux_int_le_zero_if_0(var int: x, var int: p) =
  if is_fixed(p) then
    if 0==fix(p) then x<=0 else true endif         %% 0==fix !!
  elseif lb(x)>0 then
    p==1
  elseif not (0 in dom(x)) then
    let {
      constraint assert( ub(x) < infinity,
        "aux_int_le_zero_if_0: variable \(x)'s domain: dom(\(x)) = \(dom(x)), should have finite upper bound\n" ),
      set of int: sDomNeg = dom(x) intersect -infinity..-1,
      constraint assert( card( sDomNeg ) > 0,
        "Variable \(x): dom(\(x)) = \(dom(x)), but dom() intersect -inf..-1: \(sDomNeg)\n" ),
    } in
    aux_int_le_if_0( x, max( sDomNeg ), p )
  elseif fPostprocessDomains /\ fPostproDom_AUX then
    aux_int_le_zero_if_1__POST(x, 1-p)
  elseif fMZN__UseIndicators then
    aux_int_le_zero_if_0__IND(x, p)
  else
    assert( ub(x)<infinity, "Variable \(x) needs finite upper bound for a big-M constraint, current domain \(dom(x))" ) /\
    x <= ub(x) * p
  endif;

predicate aux_float_le_zero_if_0(var float: x, var int: p) =
  %% TODO actually only need has_ub(x) in ub(x)*p but lb/ub fail on var float (not on var -infinity..const)  22.2.19
  assert( has_bounds(x), "Variable \(x) needs finite bounds for a big-M constraint" ) /\
  if is_fixed(p) then
    if 0==fix(p) then x<=0.0 else true endif
  elseif lb(x)>0.0 then
    p==1
  elseif fPostprocessDomains /\ fPostproDom_AUX then
    aux_float_le_zero_if_1__POST(x, 1-p, 1-p)
  elseif fMZN__UseIndicators then
    aux_float_le_zero_if_0__IND(x, p)
  else
    x <= ub(x) * p
  endif;

predicate aux_float_lt_zero_if_0(var float: x, var int: p) =
  assert( has_bounds(x), "Variable \(x) needs finite bounds for a big-M constraint" ) /\
  if is_fixed(p) then
    if 0==fix(p) then x<0.0 else true endif
  elseif lb(x)>=0.0 then
    p==1
  elseif fPostprocessDomains /\ fPostproDom_AUX then
    aux_float_lt_zero_if_1__POST(x, 1-p, 1-p, float_lt_EPS)
  elseif fMZN__UseIndicators then
    aux_float_le_zero_if_0__IND(x+float_lt_EPS, p)       %% Here just absolute EPS,  TODO
  else
%%    let { float: rho = float_lt_EPS_coef__ * max(abs(ub(x)), abs(lb(x))) }  % same order of magnitude as ub(x)
    let { float: rho = float_lt_EPS }          % absolute eps
    in
    %%% This one causes 2x- derivation of EPS:
    %% x < (ub(x) + rho) * p
    %%% Better?
       x <= (ub(x) + rho) * p - rho
    %%% This just uses absolute eps:
    %% x < (ub(x) + float_lt_EPS) * p
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

predicate aux_int_lt_if_1(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(x - y + 1, 1 - p);

predicate aux_int_gt_if_1(var int: x, var int: y, var int: p) =
    aux_int_le_zero_if_0(y - x + 1, 1 - p);

%% int: switching differences to float to avoid creating integer vars
/*   Used anywhere?
predicate aux_int_le_if_0(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(x - y, p);

predicate aux_int_ge_if_0(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(y - x, p);

predicate aux_int_le_if_1(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(x - y, 1 - p);

predicate aux_int_ge_if_1(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(y - x, 1 - p);

predicate aux_int_lt_if_0(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(x - y + 1.0, p);

predicate aux_int_gt_if_0(var float: x, var float: y, var int: p) =
    aux_float_le_zero_if_0(y - x + 1.0, p);

predicate aux_int_lt_if_1(var float: x, float: y, var int: p) =
    aux_float_le_zero_if_0(x - y + 1.0, 1 - p);
*/

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

predicate aux_float_gt_if_1(var float: x, var float: y, var int: p) =
    aux_float_lt_zero_if_0(y - x, 1 - p);


% -------------------------- Domains postpro ---------------------------
    %% To avoid looking if an original int var x-y exists and has eq_encode:
    %% Passing both int and float version of the indicator for flexibility:
predicate aux_float_eq_zero_if_1__POST(var float: x, var int: pI, var float: p);

predicate aux_int_le_zero_if_1__POST(var int: x, var int: p);
predicate aux_float_le_zero_if_1__POST(var float: x, var int: pI, var float: p);
predicate aux_float_lt_zero_if_1__POST(var float: x, var int: pI, var float: p, float: eps);
