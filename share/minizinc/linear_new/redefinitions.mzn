/*
% FlatZinc built-in redefinitions for linear solvers.
%
% AUTHORS
% Sebastian Brand
% Gleb Belov (2015)
%   cf. Belov, Tack, Wallace. Updated Linearization Library for MiniZinc 2.0. ModRef Workshop, CP 2015.
*/
%-----------------------------------------------------------------------------%
% Have some simple basic constraints directly in the model (better for presolve)
%   + facets as user cuts?
% Have some rarely-violated constraints as lazy cuts?
% (Symmetry constraints are both user+lazy, see redef-2.0.2.mzn)
%-----------------------------------------------------------------------------%
ann: user_cut;
ann: lazy_constraint;
%%% comment away the assignments to have those as normal constraints  
ann: MIP_cut = user_cut;     %% better in total
ann: MIP_lazy = lazy_constraint;

include "domain_encodings.mzn";

%-----------------------------------------------------------------------------%
%%% Set =true to PRINT TRACING messages for some constraints:
opt bool: mzn__my_trace_on
   = false;
   %= true;
test my_trace(string: msg) ::promise_total
  = if occurs(mzn__my_trace_on) /\ deopt(mzn__my_trace_on) then
      trace(msg)
    else true endif;
%%% Set =true to PRINT TRACING messages for the currently debugged constraints:
opt bool: mzn__my_trace__DBG_on
   = false;
   %= true;
test my_trace__DBG(string: msg) ::promise_total
  = if occurs(mzn__my_trace__DBG_on) /\ deopt(mzn__my_trace__DBG_on) then
      trace(msg)
    else true endif;

%-----------------------------------------------------------------------------%
%  Parameters
bool: fMZN__UseIndicators=false;    %% Pass on indicator constraints, CPLEX only. 
    %% Reifs give wrong result on 2012/amaze, so using implications only
%    For unary encoding: maximal domain length to invoke it
int: nMZN__UnaryLenMin__ALL=1;    %% can be used by the indiv. cases
int: nMZN__UnaryLenMax__ALL=2000;    %% can be used by the indiv. cases
int: nMZN__UnarySizeMax_intTimes=20;
int: nMZN__UnarySizeMax_cumul=2000;
int: nMZN__UnarySizeMax_1step_regular=20000;   %% network-flow decomp in the regular constraint

bool: fMZN__IgnoreRedundantCumulative=false;
%% ----- NOT WORKING NOW, use redefs_2.0.2.mzn: 
    %%%%% ----  bool: fMZN__IgnoreAllUserRedundant=false;   %% ignore all user-spec redundant constr

bool: fElementCuts=false;       %% Use simple XZ & XZB cuts for element & minimum
bool: fUseXBZCutGen=false;      %% Use XZB cut generator for element & minimum

%-----------------------------------------------------------------------------%
%  Some more detailed parameters
int: nMZN__UnaryLenMin_leq       = 1;
int: nMZN__UnaryLenMin_neq       = nMZN__UnaryLenMin__ALL;
int: nMZN__UnaryLenMin_eq        = nMZN__UnaryLenMin__ALL;
int: nMZN__UnaryLenMax_leq       = -1;
int: nMZN__UnaryLenMax_neq       = nMZN__UnaryLenMax__ALL;
int: nMZN__UnaryLenMax_eq        = nMZN__UnaryLenMax__ALL;
int: nMZN__UnaryLenMax_setIn     = nMZN__UnaryLenMax__ALL;
int: nMZN__UnaryLenMax_setInReif = nMZN__UnaryLenMax__ALL;

%-----------------------------------------------------------------------------%
% Strict inequality
%
% Uncomment the following redefinition for FlatZinc MIP solver interfaces that
% do not support strict inequality.  Note that it does not preserve equivalence
% (some solutions of the original problem may become invalid).

% predicate float_lt(var float: x, var float: y) = x + 1e-06 <= y;

%----------------------------- BOOL2INT --------------------------------%
function var bool: reverse_map(var int: x) = (x==1);
function bool: reverse_map(int: x) = (x==1);

function var int: bool2int(var bool: x) :: promise_total =
 let { var 0..1: b2i;
       constraint (x = reverse_map(b2i)) ::is_reverse_map ;
      } in 
      b2i;

predicate bool_eq(var bool: x, var bool: y) =
  bool2int(x)==bool2int(y);

%-----------------------------------------------------------------------------%
%
% Logic operations
% Use indicators for reifs           TODO
%
%-----------------------------------------------------------------------------%

predicate bool_not(var bool: p, var bool: q) =
    bool2int(p) + bool2int(q) = 1;

predicate bool_and(var bool: p, var bool: q, var bool: r) =
  my_trace("  bool_and: \(p) /\\ \(q) <-> \(r) \n") /\
  if false then
      int_lin_le_reif__IND( [-1, -1], [p, q], -2, r)
  else
    bool_and__INT(bool2int(p), bool2int(q), bool2int(r))
  endif;

predicate bool_and__INT(var int: x, var int: y, var int: z) =
  my_trace("  bool_and__INT: \(x) /\\ \(y) <-> \(z) \n") /\
    x + y <= z + 1 /\
    %% x + y >= z * 2;         % weak
    x >= z /\ y >= z;     % strong


predicate bool_or(var bool: p, var bool: q, var bool: r) =
  my_trace("  bool_or: \(p) \\/ \(q) <-> \(r) \n") /\
  if fMZN__UseIndicators then
      my_trace__DBG("  bool_or  INDIC: \(p) \\/ \(q) <-> \(r) \n") /\
      int_lin_le_reif__IND( [-1, -1], [p, q], -1, r)
  else
    let { var int: x = bool2int(p),
          var int: y = bool2int(q),
          var int: z = bool2int(r) }
    in
    my_trace("              bool_or:  0..1: \(x) \\/ \(y) <-> \(z) \n") /\
    x + y >= z /\
    % x + y <= z * 2;       % weak
    x <= z /\ y <= z   % strong
  endif;


predicate bool_xor(var bool: p, var bool: q, var bool: r) =
  if false then
      my_trace__DBG("  bool_xor  INDIC: \(p) \\/ \(q) <-> \(r) \n") /\
      int_lin_eq_reif__IND( [1, 1], [p, q], 1, r)
  else
    let { var int: x = bool2int(p),
          var int: y = bool2int(q),
          var int: z = bool2int(r) }
    in
    x <= y + z /\
    y <= x + z /\
    z <= x + y /\
    x + y + z <= 2
  endif;


predicate bool_eq_reif(var bool: p, var bool: q, var bool: r) =
    if is_fixed(r) then   % frequent case
        if fix(r) = true then p = q else bool_not(p,q) endif
    elseif false then
      my_trace__DBG("  bool_eq_reif  INDIC: \(p) \\/ \(q) <-> \(r) \n") /\
      int_lin_eq_reif__IND( [1, -1], [p, q], 0, r)
    else
      let { var int: x = bool2int(p),
            var int: y = bool2int(q),
            var int: z = bool2int(r) }
      in
      x + y <= z + 1 /\
      x + z <= y + 1 /\
      y + z <= x + 1 /\
      x + y + z >= 1
    endif;


predicate bool_ne_reif(var bool: p, var bool: q, var bool: r) =
    bool_xor(p, q, r);


predicate bool_le(var bool: p, var bool: q) =
    let { var int: x = bool2int(p),
          var int: y = bool2int(q) }
    in
  my_trace("  bool_le: \(p) <= \(q), , 0..1: \(x) <= \(y) \n") /\
    x <= y;


predicate bool_le_reif(var bool: p, var bool: q, var bool: r) =
  if false then
      my_trace__DBG("  bool_le_reif  INDIC: \(p) \\/ \(q) <-> \(r) \n") /\
      int_lin_le_reif__IND( [1, -1], [p, q], 0, r)
  else
    let { var int: x = bool2int(p),
          var int: y = bool2int(q),
          var int: z = bool2int(r) }
    in
    my_trace("  bool_le_reif: \(p) le \(q) equiv \(r), integers: \(x) le \(y) equiv \(z) \n") /\
    1 - x + y >= z /\
      %% /\ 1 - x + y <= z * 2      not needed
    1 - x <= z /\ y <= z % strong
  endif;


predicate bool_lt(var bool: p, var bool: q) =
    not p /\ q;


predicate bool_lt_reif(var bool: p, var bool: q, var bool: r) =
  my_trace("  bool_lt_reif: \(p) lt \(q) equiv \(r) \n") /\
    (not p /\ q) <-> r;

%-----------------------------------------------------------------------------%

predicate array_bool_or(array[int] of var bool: a, var bool: b) =
    if is_fixed(b) then   % frequent case
  %%%% TODO: "Cannot partially evaluate let expression" ???????????
  %      my_trace( "  array_bool_or: \(a), \(b) \n") /\
        my_trace( "  array_bool_or FIXED: " ++ show(index_set(a)) ++ ", val=" ++ show(fix(b)) ++ "\n") /\
        if fix(b) = true then
            sum(i in index_set(a))( bool2int(a[i]) ) >= 1
        else
            forall(i in index_set(a))( not a[i] )
        endif
    else
    let { var int: x = bool2int(b),
          array[1..length(a)] of var int: c =
              [ bool2int(a[i]) | i in index_set(a) ] }
    in
  my_trace( "  array_bool_or: \(a), \(b), " ++
      " array_bool_or(" ++ show(length(a)) ++ "): vars 0..1: \(c), \(x) \n") /\
  if false then
      my_trace__DBG("  array_bool_or  INDIC: " ++ show(length(a)) ++ " elements. \n") /\
      int_lin_le_reif__IND( [ -1 | i in index_set(a) ], c, -1, b)
  else
    sum(c) >= x /\
    %  sum(c) <= x * length(a)                     % weak
    forall (i in index_set(a)) (x >= c[i])        % strong 
  endif
    endif;

predicate array_bool_and(array[int] of var bool: a, var bool: b) =
    if false /*is_fixed(b)*/ then   % frequent case. Does not work -- BUG TODO
%%        my_trace( "  array_bool_and: \(a), \(b) \n") /\   TODO
        my_trace( "  array_bool_and FIXED: " ++ show(index_set(a)) ++ ", val=" ++ show(fix(b)) ++ "\n") /\
        if fix(b) = false then
            sum(i in index_set(a))( bool2int(a[i]) ) <= length(a)-1
        else
            forall(i in index_set(a))( a[i] )
        endif
    else
    let { var int: x = bool2int(b),
          array[1..length(a)] of var int: c =
              [ bool2int(a[i]) | i in index_set(a) ] }
    in
  my_trace( % "  array_bool_and: \(a), \(b), " ++
      " array_bool_and(" ++ show(length(a)) ++ "): vars 0..1: \(c), \(x) \n") /\
  if false then
      my_trace__DBG("  array_bool_and  INDIC: " ++ show(length(a)) ++ " elements. \n") /\
      int_lin_le_reif__IND( [ -1 | i in index_set(a) ], c, -length(a), b)
  else
    length(a) - sum(c) >=  1 - x /\
    % length(a) - sum(c) <= (1 - x) * length(a);     % weak
    forall (i in index_set(a)) (x <= c[i])        % strong
  endif
    endif;

% predicate array_bool_xor(array[int] of var bool: a) = .. sum(a) is odd ..
predicate array_bool_xor(array[int] of var bool: a) =
     let { var 0..(length(a)-1) div 2: m,
           var 1..((length(a)-1) div 2)*2+1: ss = sum(i in index_set(a))( bool2int(a[i]) ) }
     in
       ss == 1 + 2 * m;

predicate bool_clause(array[int] of var bool: p, array[int] of var bool: n) =
   my_trace("  bool_clause: \(p), \(n) \n") /\
 %       my_trace( "  bool_clause: " ++ show(index_set(p)) ++ ", " ++ show(index_set(n)) ++ "\n") /\
      sum(i in index_set(p))( bool2int(p[i]) )
    - sum(i in index_set(n))( bool2int(n[i]) )
    + length(n)
    >= 1;

predicate bool_lin_eq(array[int] of int: c, array[int] of var bool: x, var int: d) =
  sum(i in index_set(x))( c[i]*bool2int(x[i]) ) == d;

predicate bool_lin_eq_reif(array[int] of int: c, array[int] of var bool: x,
                          int: d, var bool: b) =           %% No var int d, sorry
    my_trace("  bool_lin_eq_reif: \(c), \(x), \(d), \(b) \n") /\
    aux_int_eq_iff_1(sum(i in index_set(x))( c[i]*bool2int(x[i]) ), d, bool2int(b));


%-----------------------------------------------------------------------------%
%
% Linear equations and inequations
% Use indicators for reifs           TODO
% Otherwise and more using unary encoding for reasonable domains
%
%-----------------------------------------------------------------------------%

predicate int_le_reif(var int: x, var int: y, var bool: b) =
  if ub(x)<=lb(y) then b==true
  elseif lb(x)>ub(y) then b==false
  elseif false then
      my_trace__DBG("  int_le_reif  INDIC: \(x) \\/ \(y) <-> \(b) \n") /\
      int_lin_le_reif__IND( [1, -1], [x, y], 0, b)
  else
    let {
      var 0..1: p = bool2int(b)
% , var int: x_y = x-y
    }
    in            %% No big difference using the binary decomposition:
%     my_trace("  int_le_reif( var " ++ show(dom(x)) ++ ", var " ++ show(dom(y))
%       ++ ", dom(x-y) = " ++ show(dom(x-y))
%       ++ ", var \(b))\n") /\
%     if card(dom(x_y))<20 then
%       let { array[int] of var int: p = eq_encode(x_y) } in
%       my_trace ("    -- by unary: dom(x-y) = " ++ show(dom(x_y)) ++ "\n") /\
%       sum ( i in dom(x_y) where i<=0 ) ( p[i] ) == bool2int(b)
%     else
      aux_int_le_if_1(x, y, p) /\
      aux_int_gt_if_0(x, y, p)
%     endif
  endif
;

predicate int_lt_reif(var int: x, var int: y, var bool: b) =
    int_le_reif(x, y - 1, b);


predicate int_ne(var int: x, var int: y) =
    my_trace("  int_ne(var " ++ show(lb(x)) ++ " .. " ++ show(ub(x))
            ++ ", var " ++ show(lb(y)) ++ " .. " ++ show(ub(y)) ++ ")\n") /\
    int_ne(x-y, 0);
%    let { var 0..1: p }       in
%    aux_int_lt_if_1(x, y, p) /\
%    aux_int_gt_if_0(x, y, p);

predicate int_ne(var int: x, int: y) =
  %%     int_eq_reif(x, y, false);
     my_trace("  int_ne(var " ++ show(lb(x)) ++ " .. " ++ show(ub(x))
            ++ ", int " ++ show(y) ++ ")\n") /\
     if y in dom(x) then 
       if y==ub(x) then
         x <= y-1
       elseif y==lb(x) then
         x >= y+1
        elseif card(dom(x))<nMZN__UnaryLenMax_neq then                          %% PARAM TODO
          let {
            array[int] of var int: p = eq_encode(x);
          } in
          p[y]==false
        else int_ne__SIMPLE(x, y)
       endif
     else
       true
     endif;

predicate int_ne__SIMPLE(var int: x, var int: y) =
  if false then
%    let { var bool: fEq; } in
%      int_lin_eq_reif__IND( [1, -1], [x, y], 0, fEq) /\
%      fEq = false    %% -- this is how CPLEX 12.6.2 needs it, not in the BOUNDS section   TODO?
      my_trace__DBG("  int_ne  INDIC: \(x) \\/ \(y)  \n") /\
      int_lin_ne__IND( [1, -1], [x, y], 0)
  else
    let { var 0..1: p }
    in
    aux_int_lt_if_1(x, y, p) /\
    aux_int_gt_if_0(x, y, p)
  endif;


predicate int_lin_ne(array[int] of int: c, array[int] of var int: x, int: d) =
  if false then
%    let { var bool: fEq; } in
%      int_lin_eq_reif__IND( c, x, d, fEq ) /\
%      fEq = false    %% -- this is how CPLEX 12.6.2 needs it, not in the BOUNDS section   TODO?
      my_trace__DBG("  int_lin_ne  INDIC: " ++ show(length(c)) ++ "  elements. \n") /\
      int_lin_ne__IND( c, x, d )
  else
    int_ne(sum(i in index_set(x))( c[i]*x[i] ),d)
  endif;

predicate int_eq_reif(var int: x, var int: y, var bool: b) =
    my_trace("  int_eq_reif ( x: " ++ if has_bounds(x) then show(dom(x)) else "int" endif
      ++ ",  y: " ++ if has_bounds(y) then show(dom(y)) else "int" endif ++ " )\n" ) /\
    if card(dom(x) intersect dom(y))>0 then
      if is_fixed(x) then
        if is_fixed(y) then
          b <-> fix(x)==fix(y)
        else
          int_eq_reif(y, fix(x), b)
        endif
      elseif is_fixed(y) then
        int_eq_reif(x, fix(y), b)
      else
        int_eq_reif(x-y, 0, b)
%%%        aux_int_eq_iff_1(x, y, bool2int(b))
      endif
    else
      not b
    endif;

predicate int_eq_reif(var int: x, int: y, var bool: b) =
  if y in dom(x) then
    my_trace("  int_eq_reif ( x: " ++ if has_bounds(x) then show(dom(x)) else "int" endif
      ++ ",  y=const: " ++ show(y) ++ " )\n") /\
    if y==lb(x) then
      int_lt_reif(y, x, not b)
    elseif y==ub(x) then
      int_lt_reif(x, y, not b)
    elseif card(dom(x))<nMZN__UnaryLenMax_eq then                                 %% PARAM TODO. That low (10) is needed for l2p
      let {
        array[int] of var int: p = eq_encode(x);
      } in
        p[y]==b
    else
      aux_int_eq_iff_1(x, y, bool2int(b))
    endif
   else
     b==false
   endif;
    

predicate int_ne_reif(var int: x, var int: y, var bool: b) =
    int_eq_reif(x, y, not b);

predicate int_ne_reif(var int: x, int: y, var bool: b) =
    int_eq_reif(x, y, not b);

%-----------------------------------------------------------------------------%

predicate int_lin_eq_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
    my_trace("  int_lin_eq_reif: \(c), \(x), \(d), \(b) \n" ) /\
    int_eq_reif(sum(i in index_set(x))( c[i]*x[i] ), d, b);
    %% aux_int_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d, bool2int(b));
    %% -- use eq_encode as well? TODO


predicate int_lin_ne_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
    my_trace("  int_lin_ne_reif: \(c), \(x), \(d), \(b) \n") /\
    int_ne_reif(sum(i in index_set(x))( c[i]*x[i] ), d, b);
%    aux_int_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d, 1 - bool2int(b));


predicate int_lin_le_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
%    my_trace("  int_lin_le_reif: \(c), \(x), \(d), \(b) \n") /\
%    my_trace("  int_lin_le_reif: coefs = " ++ show(c) ) /\
%    my_trace("  int_lin_le_reif: lb's  = " ++ show([lb(x[i]) | i in index_set(x)]) ) /\
%    my_trace("  int_lin_le_reif: ub's  = " ++ show([ub(x[i]) | i in index_set(x)]) ++ "\n" ) /\
    int_le_reif(sum(i in index_set(x))( c[i] * x[i] ), d, b);
%    let { var 0..1: p = bool2int(b) }    in
%    aux_int_le_if_1(sum(i in index_set(x))( c[i] * x[i] ), d, p) /\
%    aux_int_gt_if_0(sum(i in index_set(x))( c[i] * x[i] ), d, p);


predicate int_lin_lt_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
    int_lin_le_reif(c, x, d - 1, b);

%-----------------------------------------------------------------------------%

predicate float_le_reif(var float: x, var float: y, var bool: b) =
  if false then
    my_trace__DBG("  float_le_reif  INDIC: \(x) \\/ \(y) <-> \(b) \n") /\
    float_lin_le_reif__IND( [1.0, -1.0], [x, y], 0.0, b)
  else
    let { var int: p = bool2int(b) }
    in
    aux_float_le_if_1(x, y, (p)) /\
    aux_float_gt_if_0(x, y, (p))
  endif;


predicate float_lt_reif(var float: x, var float: y, var bool: b) =
    let { var int: p = bool2int(b) }
    in
    aux_float_lt_if_1(x, y, (p)) /\
    aux_float_ge_if_0(x, y, (p));


predicate float_ne(var float: x, var float: y) =
    let { var 0..1: p }
    in
    aux_float_lt_if_1(x, y, (p)) /\
    aux_float_gt_if_0(x, y, (p));

%% TODO Why is disequality with EPS but equality not??
    
predicate float_eq_reif(var float: x, var float: y, var bool: b) =
    aux_float_eq_iff_1(x, y, (bool2int(b)));


predicate float_ne_reif(var float: x, var float: y, var bool: b) =
    aux_float_eq_iff_1(x, y, 1 - (bool2int(b)));

%-----------------------------------------------------------------------------%

predicate float_lin_eq_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
    aux_float_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d,
        (bool2int(b)));


predicate float_lin_ne_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
    aux_float_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d,
        1 - (bool2int(b)));


predicate float_lin_le_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
  if false then
    my_trace__DBG("  float_lin_le_reif__IND  INDIC: " ++ show(length(c)) ++ " elements. \n") /\
    float_lin_le_reif__IND( c, x, d, b)
  else
    let { var int: p = (bool2int(b)) }
    in
    aux_float_le_if_1(sum(i in index_set(x))( c[i] * x[i] ), d, p) /\
    aux_float_gt_if_0(sum(i in index_set(x))( c[i] * x[i] ), d, p)
  endif;


predicate float_lin_lt_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
    let { var int: p = (bool2int(b)) }
    in
    aux_float_lt_if_1(sum(i in index_set(x))( c[i] * x[i] ), d, p) /\
    aux_float_ge_if_0(sum(i in index_set(x))( c[i] * x[i] ), d, p);

%-----------------------------------------------------------------------------%
% Minimum, maximum, absolute value
% Use unary as well? TODO

predicate int_abs(var int: x, var int: z) =
%% The simplifications seem worse on   league.mzn	model90-18-20.dzn:
%% but the .lp seem to differ just by order...?? TODO
  if lb(x)>=0 then z==x
  elseif ub(x)<=0 then z==-x
  else
    let { var 0..1: p }
    in
    % z <= x \/ z <= -x
    aux_int_le_if_1(z,  x, p) /\
    aux_int_le_if_0(z, -x, p) /\
    z >=  x /\
    z >= -x /\
    z >= 0 /\                         % This is just for preprocessor
    z <= max([ub(x), -lb(x)])        % And this
  endif
  ;

predicate int_min(var int: x, var int: y, var int: z) =
    array_int_minimum(z, [x, y]);

predicate int_max(var int: x, var int: y, var int: z) =
    array_int_maximum(z, [x, y]);

predicate float_abs(var float: x, var float: z) =
    let { var 0..1: p }
    in
    % z <= x \/ z <= -x
    aux_float_le_if_1(z,  x, (p)) /\
    aux_float_le_if_0(z, -x, (p)) /\
    z >=  x /\
    z >= -x /\
    z >= 0.0 /\                       % This is just for preprocessor
    z <= max([ub(x), -lb(x)]);        % And this

predicate float_min(var float: x, var float: y, var float: z) =
    array_float_minimum(z, [x, y]);

predicate float_max(var float: x, var float: y, var float: z) =
    array_float_maximum(z, [x, y]);

%-----------------------------------------------------------------------------%
% Multiplication and division

predicate int_div(var int: x, var int: y, var int: q) =
    let { 
      int: bx = max(abs(lb(x)), abs(ub(x)));
      constraint -bx<=q /\ q<=bx;
      int: by = max(abs(lb(y)), abs(ub(y)));
      var -by+1..by-1: r;
    }
    in
    aux_int_division_modulo(x,y,q,r);


predicate int_mod(var int: x, var int: y, var int: r) =
    let { 
      int: bx = max(abs(lb(x)), abs(ub(x)));
      var -bx..bx: q;
      int: by = max(abs(lb(y)), abs(ub(y)));
      constraint -by+1<=r /\ r<=by-1;
    }
    in
    aux_int_division_modulo(x,y,q,r);


predicate aux_int_division_modulo(var int: x, var int: y, var int: q,
        var int: r) =
    let {
      int: bx = max(abs(lb(x)), abs(ub(x)));
      constraint -bx<=q /\ q<=bx;
      int: by = max(abs(lb(y)), abs(ub(y)));
      constraint -by+1<=r /\ r<=by-1;
      var 0..1: p
    }
    in
    x = y * q + r /\
    % 0 <= x -> 0 <= r    which is    0 > x \/ 0 <= r
    aux_int_lt_if_1(x, 0, p) /\
    aux_int_ge_if_0(r, 0, p) /\
    % x < 0 -> r <= 0    which is    x >= 0 \/ r <= 0
    aux_int_ge_if_0(x, 0, p) /\
    aux_int_le_if_1(r, 0, p) /\
    % abs(r) < abs(y)
    let { var 1.. max(abs(lb(y)), abs(ub(y))): w = abs(y) }
    in
    w >  r /\
    w > -r;

%% Can also have int_times(var float, var int) ......... TODO

predicate int_times(var int: x, var int: y, var int: z) =
if is_fixed(x) then
  z==fix(x)*y             %%%%% Need to use fix() otherwise added to map & nothing happens
elseif is_fixed(y) then
  z==x*fix(y)
  else
  if 0..1==dom(x) /\ 0..1==dom(y) then bool_and__INT(x,y,z)
  elseif card(dom(x))==2 /\ card(dom(y))==2 /\ 0 in dom(x) /\ 0 in dom(y)
    then let {
      var 0..1: xn;
      var 0..1: yn;
      var 0..1: zn;
      constraint x=xn*max(dom(x) diff {0});
      constraint y=yn*max(dom(y) diff {0});
      constraint z=zn*max(dom(x) diff {0})*max(dom(y) diff {0});
    } in
    bool_and__INT(xn,yn,zn)
  elseif card(dom(x)) * card(dom(y)) > nMZN__UnarySizeMax_intTimes then                    %% PARAM
    %% ALSO NO POINT IF <=4.          TODO
    if card(dom(x)) > card(dom(y)) then int_times(y,x,z)
    else
       my_trace ( " int_times(x,y,z): dom(x) = " ++ show(dom(x)) ++ "  " ) /\
       my_trace ( ", dom(y) = " ++ show(dom(y)) ++ "\n" ) /\
      let {
          set of int: s = lb(x)..ub(x),
          set of int: r = {lb(x)*lb(y), lb(x)*ub(y), ub(x)*lb(y), ub(x)*ub(y)},
          array[s] of var min(r)..max(r): ady = array1d(s, [ d*y | d in s ]) }
      in
      ady[x] = z                             %% use element()
    endif
  else
    int_times_unary(x, { }, y, z)
  endif
endif;

%% domx__ can be used to narrow domain... NOT IMPL.
predicate int_times_unary(var int: x, set of int: domx__, var int: y, var int: z) =
  let {
    set of int: r = {lb(x)*lb(y), lb(x)*ub(y), ub(x)*lb(y), ub(x)*ub(y)},
    %% set of int: domx = if card(domx__)>0 then domx__ else dom(x) endif,
    array[int, int] of var int: pp=eq_encode(x, y)
  } in
  my_trace ( " int_times_unary(\(x),\(y),\(z)): dom(x) = " ++ show(dom(x)) ++ "  " ) /\
    %  my_trace ( ", domx = " ++ show(domx) ++ "  " ) /\
    my_trace ( ", dom(y) = " ++ show(dom(y)) ++ "  " ) /\
    z>=min(r) /\ z<=max(r) /\
    my_trace ( ", dom(z) = " ++ show(dom(z)) ++ "\n" ) /\    %% after the bounds
    z==sum(i in index_set_1of2(pp), j in index_set_2of2(pp))
         (i * j * pp[i, j]) /\
    forall(i in index_set_1of2(pp), j in index_set_2of2(pp)
      where not ((i*j) in dom(z))
    )(pp[i, j]==0)
  ;


predicate int_times_unary__NOFN(var int: x, set of int: domx__, var int: y, var int: z) =
      let {
        set of int: r = {lb(x)*lb(y), lb(x)*ub(y), ub(x)*lb(y), ub(x)*ub(y)},
        %% set of int: domx = if card(domx__)>0 then domx__ else dom(x) endif,
        array[int] of var int: pX = eq_encode(x),
        array[int] of var int: pY = eq_encode(y),
        array[int] of int: valX = [ v | v in index_set(pX) ],       %% NOT domx.
        array[int] of int: valY = [ v | v in index_set(pY) ],       %% -- according to eq_encode!
        array[index_set(valX), index_set(valY)] of var 0..1: pp   %% both dim 1..
      } in
    if is_fixed(x) \/ is_fixed(y) then
      z==x*y
    else
     my_trace ( " int_times_unary__NOFN(\(x),\(y),\(z)): dom(x) = " ++ show(dom(x)) ++ "  " ) /\
     %  my_trace ( ", domx = " ++ show(domx) ++ "  " ) /\
     my_trace ( ", dom(y) = " ++ show(dom(y)) ++ "  " ) /\
        z>=min(r) /\ z<=max(r) /\
     my_trace ( ", dom(z) = " ++ show(dom(z)) ++ "\n" ) /\    %% after the bounds
        sum(pp)==1 /\
        z==sum(i in index_set(valX), j in index_set(valY))
             (valX[i] * valY[j] * pp[i, j]) /\
        forall(i in index_set(valX))
          ( pX[valX[i]] == sum(j in index_set(valY))( pp[i, j] ) ) /\
        forall(j in index_set(valY))
          ( pY[valY[j]] == sum(i in index_set(valX))( pp[i, j] ) )
    endif;


%-----------------------------------------------------------------------------%
% Array 'element' constraints

predicate array_bool_element(var int: x, array[int] of bool: a, var bool: z) =
    array_int_element(x, arrayXd(a, [bool2int(a[i]) | i in index_set(a)]), bool2int(z));

predicate array_var_bool_element(var int: x, array[int] of var bool: a,
                                 var bool: z) =
    array_var_int_element(x, arrayXd(a, [bool2int(a[i]) | i in index_set(a)]), bool2int(z));

predicate array_int_element(var int: x, array[int] of int: a, var int: z) =
    array_float_element(x, arrayXd(a, [int2float(a[i]) | i in index_set(a)]), int2float(z));

predicate array_var_int_element(var int: x, array[int] of var int: a,
                                var int: z) =
   %%%% Relate to the float version:
    array_var_float_element(x, arrayXd(a, [int2float(a[i]) | i in index_set(a)]), int2float(z));
   %%%% Simplistic version:
   %%%% Complete binarization: MEMORY FULL. Need exact domains & sparse encoding


predicate array_float_element(var int: i00, array[int] of float: a,
                              var float: z) =
    let { set of int: ix = index_set(a),
          constraint i00 in ix
    } in
    z >= min(i in dom(i00))(a[i]) /\
    z <= max(i in dom(i00))(a[i]) /\
    if card(dom(i00))==1 \/ min(i in dom(i00))(a[i])==max(i in dom(i00))(a[i])
      then true           %%%% z==a[fix(i00)]
    else
      let {
        int: nUBi00 = max(dom(i00)),
        int: nLBi00 = min(dom(i00)),
        float: nMinDist = min(i in nLBi00 .. nUBi00-1)(a[i+1]-a[i]),
        float: nMaxDist = max(i in nLBi00 .. nUBi00-1)(a[i+1]-a[i]),
      } in
      if nMinDist == nMaxDist then               %% The linear case
        z == a[nLBi00] + nMinDist*(i00-nLBi00)
      else
        let {
          array[int] of var int: p = eq_encode(i00)    %% this needs i00 in ix
        }
        in
        my_trace("  array_float_element(i00, a[], z:  dom(i00) = " ++ show(dom(i00))
        ++ "\n     z in: " ++ show(lb(z)) ++ " .. " ++ show(ub(z))
        ++ "\n     elements of a in: " ++ show(min(i in dom(i00))(a[i])) ++ " .. " ++ show(max(i in dom(i00))(a[i]))
        ++ "\n     index_set(a) = " ++ show(index_set(a))
        ++ "\n     index_set(p) = " ++ show(index_set(p))
        ++ "\n") /\
        assert(dom(i00) subset index_set(p), "", true) /\
        sum(i in dom(i00))( a[i] * int2float(p[i]) ) == z        %% add more hull?
      endif
    endif;

predicate array_var_float_element(var int: i00, array[int] of var float: a,
                                  var float: z) =
    let { set of int: ix = index_set(a),
          constraint i00 in ix,
          float: minLB=min(i in dom(i00))(lb(a[i])),
          float: maxUB=max(i in dom(i00))(ub(a[i]))
    } in
          z >= minLB /\
          z <= maxUB /\
    if is_fixed(i00) then z==a[fix(i00)]
    elseif minLB==maxUB then true
    else
      let {
        array[int] of var int: p = eq_encode(i00),
      } in
      my_trace (
          "  array_var_float_element: i in "
                ++ show(lb(i00)) ++ ".." ++ show(ub(i00)) ++
            "\n  LB(a) = " ++ show([lb(a[i]) | i in ix]) ++
            "\n  UB(a) = " ++ show([ub(a[i]) | i in ix]) ++
            ",  z in " ++ show(lb(z)) ++ ".." ++ show(ub(z)) ++ "\n") /\
      assert(dom(i00) subset index_set(p), "", true) /\
  %%% The convexified bounds seem slow for ^2 and ^3 equations:
  %    sum(i in dom(i01))( lb(a[i]) * int2float(p[i]) ) <= z /\    %% convexify lower bounds
  %    sum(i in dom(i01))( ub(a[i]) * int2float(p[i]) ) >= z /\    %% convexify upper bounds
      forall (i in dom(i00))(
          aux_float_eq_if_1(z, a[i], p[i])
      )
      %% Cuts:
        /\ 
        if fElementCuts then
          array_var_float_element__ROOF([  a[i] | i in dom(i00) ],  z)  :: MIP_cut   %% these 2 better as user cuts - too slow
          /\ array_var_float_element__ROOF([ -a[i] | i in dom(i00) ], -z)  :: MIP_cut   %% or even skip them
          /\    array_var_float_element__XBZ_lb([  a[i] | i in dom(i00) ],  [  p[i] | i in dom(i00) ],  z)  :: MIP_cut
          /\    array_var_float_element__XBZ_lb([ -a[i] | i in dom(i00) ],  [  p[i] | i in dom(i00) ], -z)  :: MIP_cut
        else true
        endif
    endif ;

%%% Facets on the upper surface of the z-a polytope
%%% Possible parameter: maximal number of first cuts taken only
predicate array_var_float_element__ROOF(array[int] of var float: a,
                                  var float: z) =
    let { set of int: ix = index_set(a),
          int: n = length(a),
%          array[int] of float: ALm = [ -lb(a[i]) | i in 1..n],
%          array[int] of int: srt_lb = sort_by([i | i in 1..n], ALm),
                                            %indices of lb sorted down
          array[int] of float: AU = [ ub(a[i]) | i in 1..n],
          array[int] of int: srt_ub = sort_by([i | i in 1..n], AU),
                                            %indices of ub sorted up
          array[int] of float: AU_srt_ub = [ub(a[srt_ub[i]]) | i in 1..n],
          array[int] of float: AL_srt_ub = [lb(a[srt_ub[i]]) | i in 1..n],
          array[int] of float: MaxLBFrom =
            [ max(j in index_set(AL_srt_ub) where j>=i)(AL_srt_ub[j])
              | i in 1..n ], %% direct, O(n^2)
          array[int] of float: ULB = [
              if 1==i then MaxLBFrom[1]
              else max([AU_srt_ub[i-1], MaxLBFrom[i]])
              endif | i in 1..n ]
%          array[int] of float: AL_srt = [AL[srt[i]] | i in 1..n],
%          array[int] of float: AU_srt = [ub(x[srt[i]]) | i in 1..n]
    } in
       %%% "ROOF"
    forall (i in 1..n where
          if i==n then true else ULB[i]!=ULB[i+1] endif        %% not the same base bound
        )(
      z <= ULB[i]
           + sum( j in i..n where AU_srt_ub[i] != AL_srt_ub[i] )        %% not a const
             ( (AU_srt_ub[j]-ULB[i]) * (a[srt_ub[j]]-AL_srt_ub[j]) / (AU_srt_ub[j]-AL_srt_ub[j]) ) )
    ;

predicate array_var_float_element__XBZ_lb(array[int] of var float: x, array[int] of var int: b, var float: z) =
   %% Adding some cuts a priori, also to make solver extract the variables
    (z <= sum(i in index_set(x))(ub(x[i]) * b[i])) %:: MIP_cut   -- does not work to put them here TODO
    /\
    forall(i in index_set(x))
      ( assert(lb(x[i]) == -ub(-x[i]) /\ ub(x[i]) == -lb(-x[i]), " negated var's bounds should swap " ) /\
        z <= x[i] + sum(j in index_set(x) where i!=j)((ub(x[j])-lb(x[i]))*b[j])) %:: MIP_cut   %% (ub_j-lb_i) * b_j
    /\
    forall(i in index_set(x))
      ( z <= ub(x[i])*b[i] + sum(j in index_set(x) where i!=j)(x[j]+lb(x[j])*(b[j]-1)) ) %:: MIP_cut   %% (ub_j-lb_i) * b_j
    /\
    (z <= sum(i in index_set(x))(x[i] + lb(x[i]) * (b[i]-1))) %:: MIP_cut
    /\
    if fUseXBZCutGen then
%           my_trace__DBG("  % array_float_minimum_I: calling array_var_float_element__XBZ_lb: lb(-x), ub(-x), lb(b), ub(b):\n  % "
%             ++ show(lb([ -x[i] | i in sActive ])) ++ "\n  % "
%             ++ show(ub([ -x[i] | i in sActive ])) ++ "\n  % "
%             ++ show(lb([ p[i] | i in sActive ])) ++ "\n  % "
%             ++ show(ub([ p[i] | i in sActive ])) ++ "\n  %  --------------------------------------\n"
%          ) /\
      array_var_float_element__XBZ_lb__cutgen(x, b, z)   :: MIP_cut
    else true
    endif;

%-----------------------------------------------------------------------------%
% Set constraints

predicate array_var_set_element(var int: x, array[int] of var set of int: y, var set of int: z) =
%  let {
%    int: yLB = min([min(ub[i]]),
%  } in
  forall (k in ub(z)) (
    if k in array_union(y) then
      set2bools(z)[k] == 
        [ if k in ub(y[i]) then set2bools(y[i])[k] else false endif | i in index_set(y) ][x]
    else
      set2bools(z)[k] == false
    endif
  );

predicate array_set_element(var int: x, array[int] of set of int: y, var set of int: z) =
%  let {
%    int: yLB = min([min(ub[i]]),
%  } in
  forall (k in ub(z)) (
    if k in array_union(y) then
      set2bools(z)[k] == 
        [ if k in ub(y[i]) then set2bools(y[i])[k] else false endif | i in index_set(y) ][x]
    else
      set2bools(z)[k] == false
    endif
  );

% XXX  only for a fixed set here,  general see below:

predicate set_in(var int: x, set of int: s__) =
    my_trace("  set_in ( x: " ++ if has_bounds(x) then show(dom(x)) else "int" endif ++ ", s__: " ++ show(s__) ++ " )\n" ) /\
    let {
      set of int: s = if has_bounds(x) then s__ intersect dom(x) else s__ endif,
      constraint  min(s) <= x /\ x <= max(s);
    } in
    if s = min(s)..max(s) then true
    else
%      if card(s intersect dom(x))<=nMZN__UnaryLenMax_setIn then               %% PARAM TODO
%        let {
%          array[int] of var int: p = eq_encode(x);
%        } in
%          sum(i in s intersect dom(x))(p[i]) == 1 /\
%          forall(i in dom(x) diff s)(p[i]==0)
%      else
%        1 == fVarInBigSetOfInt(x, s)
      let {
        array[int] of int: sL = [ e | e in s where not (e - 1 in s) ];
        array[int] of int: sR = [ e | e in s where not (e + 1 in s) ];
        array [index_set(sR)] of var 0..1: B;
        constraint assert(length(sR)==length(sL), "N of lb and ub of sub-intervals of a set should be equal");
      } in    
        sum(B) = 1      %% use indicators
        /\
        x >= sum(i in index_set(sL))(B[i]*sL[i])     
        /\ 
        x <= sum(i in index_set(sR))(B[i]*sR[i])
%      endif
%            1 == sum(e in s)(bool2int( x == e ))
%        exists(e in s)( x = e )
%        let { var 1..card(s): i } in
%        array_int_element(i, [v | v in s], x)
    endif;

% XXX  only for a fixed set
predicate set_in_reif(var int: x, set of int: s__, var bool: b) =
    my_trace("  set_in_reif ( x: " ++ if has_bounds(x) then show(dom(x)) else "int" endif ++ ", s__: " ++ show(s__) ++ " )\n" ) /\
    let {
      set of int: s = if has_bounds(x) then s__ intersect dom(x) else s__ endif,
    } in
    (
         if dom(x) subset s then b==true
         elseif card(dom(x) intersect s)==0 then b==false
         elseif s = min(s)..max(s) then
             b <-> (min(s) <= x /\ x <= max(s))
         else
%            let {
%              array[int] of int: sL = [ e | e in s where not (e - 1 in s) ];
%              array[int] of int: sR = [ e | e in s where not (e + 1 in s) ];
%              array [index_set(sR)] of var 0..1: B;
%              constraint assert(length(sR)==length(sL), "N of lb and ub of sub-intervals of a set should be equal");
%            } in    
%              sum(B) = bool2int(b)      %% use indicators
%              /\
%              x >= lb(x) + sum(i in index_set(sL))(B[i]*(sL[i]-lb(x)))     
%              /\ 
%              x <= ub(x) + sum(i in index_set(sR))(B[i]*(sR[i]-ub(x)))
           aux_int_le_if_1(x, max(s), bool2int(b)) /\
           aux_int_ge_if_1(x, min(s), bool2int(b)) /\
            if card(dom(x))<=nMZN__UnaryLenMax_setInReif then               %% PARAM TODO
              let {
                array[int] of var int: p = eq_encode(x);
              } in
                sum(i in s intersect dom(x))(p[i]) == bool2int(b)
            else
              bool2int(b) == fVarInBigSetOfInt(x, s)
            endif
%           bool2int(b) == sum(e in s)(bool2int( x == e ))
        endif
    );

    % Alternative
predicate alt_set_in_reif(var int: x, set of int: s, var bool: b) =
    b <->
        exists(i in 1..length([ 0 | e in s where not (e - 1 in s) ]))(
            let { int: l = [ e | e in s where not (e - 1 in s) ][i],
                  int: r = [ e | e in s where not (e + 1 in s) ][i] }
            in
            l <= x /\ x <= r
        );

function var int: fVarInBigSetOfInt(var int: x, set of int: s)
  = let {
    array[int] of int: sL = [ e | e in s where not (e - 1 in s) ];
    array[int] of int: sR = [ e | e in s where not (e + 1 in s) ];
    constraint assert(length(sR)==length(sL), "N of lb and ub of sub-intervals of a set should be equal");
  } in
    sum(i in index_set(sL)) (bool2int(x>=sL[i] /\ x<=sR[i]));      %% use indicators
        
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% OTHER SET STUFF, COPIED FROM ../nosets/redefs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function var set of int: reverse_map(array[int] of var bool: b);
function set of int: reverse_map(array[int] of bool: b) ::promise_total =
  { i | i in index_set(b) where b[i] };

array[int] of var bool: set2bools(var set of int: x) ::promise_total =
  if is_fixed(x) then set2bools(fix(x)) else
  let {
    array[min(ub(x))..max(ub(x))] of var bool: b;
    constraint forall (i in index_set(b) where not (i in ub(x))) (b[i]=false);
    constraint (x = reverse_map(b)) :: is_reverse_map;
    constraint   my_trace("  set2bools(\(x)) = \(b) \n")
  } in b
  endif;
array[int] of var bool: set2bools(var set of int: x, set of int: ubx) ::promise_total =
  if is_fixed(x) then set2bools(fix(x) intersect ubx) else
  let {
    array[min(ubx)..max(ubx)] of var bool: b;
    constraint forall (i in index_set(b) where not (i in ubx)) (b[i]=false);
    constraint (x = reverse_map(b)) :: is_reverse_map;
    constraint   my_trace("  set2bools(\(x), \(ubx)) = \(b) \n")
  } in b
  endif;
array[int] of bool: set2bools(set of int: x) ::promise_total =
  array1d(min(x)..max(x),[i in x | i in min(x)..max(x)]);
array[int] of bool: set2bools(set of int: x, set of int: ubx) ::promise_total =
  set2bools(x intersect ubx);

predicate set_eq(var set of int: x, var set of int: y) =
  if not has_ub_set(x) /\ not has_ub_set(y) then
    assert(false, "Cannot determine bounds of set variables")
  elseif not has_ub_set(x) then set_eq(y,x)
  else
  let {
    set of int: uby = if has_ub_set(y) then ub(y) else ub(x) endif;
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y,uby);
  } in forall (i in index_set(bx) union index_set(by)) (
    if not (i in index_set(bx)) then not by[i]
    elseif not (i in index_set(by)) then not bx[i]
    else bx[i]=by[i] endif
  )
  endif;

predicate set_eq_reif(var set of int: x, var set of int: y, var bool: b) =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
  } in b <-> forall (i in index_set(bx) union index_set(by)) (
    if not (i in index_set(bx)) then not by[i]
    elseif not (i in index_set(by)) then not bx[i]
    else bx[i]=by[i] endif
  );

predicate set_subset(var set of int: x, var set of int: y) =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
  } in forall (i in index_set(bx)) (
    if not (i in index_set(by)) then not bx[i]
    else bx[i] -> by[i] endif
  );

predicate set_subset_reif(var set of int: x, var set of int: y, var bool: b) =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
  } in b <-> forall (i in index_set(bx)) (
    if not (i in index_set(by)) then not bx[i]
    else bx[i] -> by[i] endif
  );

function var set of int: set_intersect(var set of int: x, var set of int: y) ::promise_total =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
    var set of (index_set(bx) intersect index_set(by)): z;
    array[int] of var bool: bz = set2bools(z);
    constraint forall (i in index_set(bz)) (
      bz[i] = (bx[i] /\ by[i])
    );
  } in z;
  
%predicate set_le(var set of int: x, var set of int: y) ::promise_total =
%  let {
%    array[int] of var bool: bx = set2bools(x);
%    array[int] of var bool: by = set2bools(y);
%  } in 

function var set of int: set_union(var set of int: x, var set of int: y) ::promise_total =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
    var set of (index_set(bx) union index_set(by)): z;
    array[int] of var bool: bz = set2bools(z);
    constraint forall (i in index_set(bx) union index_set(by)) (
      if (i in index_set(bx)) then
        if (i in index_set(by)) then
          bz[i] = (bx[i] \/ by[i])
        else
          bz[i] = bx[i]
        endif
      else
          bz[i] = by[i]
      endif
    );
  } in z;

function var set of int: set_diff(var set of int: x, var set of int: y) ::promise_total =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
    var set of (index_set(bx) diff index_set(by)): z;
    array[int] of var bool: bz = set2bools(z);
    constraint forall (i in index_set(bz)) (
      bz[i] = (bx[i] /\ (not by[i]))
    );
  } in z;

function var int: card(var set of int: x) ::promise_total =
  let {
    array[int] of var bool: bx = set2bools(x);
    var 0..length(bx) : c;
    constraint bool_lin_eq([1 | i in index_set(bx)],bx,c);
  } in c;

predicate set_in(var int: x, var set of int: y) =
  let {
    array[int] of var bool: by = set2bools(y);
  } in by[x];

predicate set_in_reif(var int: x, var set of int: y, var bool: b) =
  let {
    array[int] of var bool: by = set2bools(y);
  } in b <-> by[x];

%function array[int] of var set of int: reverse_map(array[int] of var bool: b);
%%%% Use array[int,int] of var bool?
%function set of int: reverse_map(array[int] of bool: b) ::promise_total =
%  { i | i in index_set(b) where b[i] };

%  let {
%    array[min(ub(x))..max(ub(x))] of var bool: b;
%    constraint forall (i in index_set(b) where not (i in ub(x))) (b[i]=false);
%    constraint (x = reverse_map(b)) :: is_reverse_map;
%  } in b
function array[int] of var bool: setarray2bools(array[int] of var set of int: x)
  :: promise_total
  =
  if length(x)=0 then [] else
    set2bools(x[1])++setarray2bools([x[i]|i in 2..length(x)])
  endif;
%function array[int] of var bool: setarray2bools(array[int] of var set of int: x)
%  :: promise_total
%  = let {
%    array[int] of var bool: b =
%      if length(x)=0 then [] else
%        set2bools(x[1])++setarray2bools_I([x[i]|i in 2..length(x)])
%      endif;
%    constraint (x = reverse_map(b)) :: is_reverse_map;
%  } in b;

annotation set_search(array[int] of var set of int: x, ann: a1, ann: a2, ann: a3) =
  bool_search(setarray2bools(x),a1,a2,a3);


%-----------------------------------------------------------------------------%
% Auxiliary: equality reified onto a 0/1 variable

predicate aux_int_eq_iff_1(var int: x, var int: y, var int: p) =
  if false then
    my_trace__DBG("  aux_int_eq_iff_1  INDIC: \(x) \\/ \(y) <-> \(p) \n") /\
    int_lin_eq_reif__IND([1, -1], [x, y], 0, (p==1))
  else
    let { array[1..2] of var 0..1: q }
    in
%  my_trace("  aux_int_eq_iff_1: var " ++ show(dom(x))
%              ++ ", var " ++ show(dom(y)) ++ "\n") /\
    aux_int_le_if_1(x, y, p) /\
    aux_int_ge_if_1(x, y, p) /\
    aux_int_lt_if_0(x, y, q[1]) /\
    aux_int_gt_if_0(x, y, q[2]) /\
    sum(q) == p + 1
  endif;

    % Alternative 2
predicate aux_int_eq_iff_1__WEAK1(var int: x, var int: y, var int: p) =
    let { array[1..2] of var 0..1: q_458 }
    in
    aux_int_lt_if_0(x - p, y, q_458[1]) /\
    aux_int_gt_if_0(x + p, y, q_458[2]) /\
    sum(q_458) <= 2 - 2*p /\
    sum(q_458) <= 1 + p;

    % Alternative 1
predicate alt_1_aux_int_eq_iff_1(var int: x, var int: y, var int: p) =
    let { array[1..2] of var 0..1: q }
    in
    aux_int_lt_if_0(x - p, y, q[1]) /\
    aux_int_gt_if_0(x + p, y, q[2]) /\
    q[1] <= 1 - p /\
    q[2] <= 1 - p /\
    sum(q) <= 1 + p;

predicate aux_float_eq_iff_1(var float: x, var float: y, var int: p) =
%%  if fMZN__UseIndicators then   ??????
    let { array[1..2] of var 0..1: q }
    in
    aux_float_le_if_1(x, y, p) /\
    aux_float_ge_if_1(x, y, p) /\
    aux_float_lt_if_0(x, y, (q[1])) /\
    aux_float_gt_if_0(x, y, (q[2])) /\
    sum(i in 1..2)((q[i])) == 1 + p;

predicate aux_float_eq_if_1(var float: x, var float: y, var int: p) =
  if fMZN__UseIndicators then
    my_trace__DBG("  aux_float_eq_if_1  INDIC: \(x) \\/ \(y) <-> \(p) \n") /\
    aux_float_eq_if_1__IND(x, y, p)
  else
    aux_float_le_if_1(x, y, p) /\
    aux_float_ge_if_1(x, y, p)
  endif;

%-----------------------------------------------------------------------------%
% Auxiliary: indicator constraints
%   p -> x # 0  where p is a 0/1 variable and # is a comparison

% Base cases

predicate aux_int_le_zero_if_0(var int: x, var int: p) =
%    my_trace("  aux_int_le_zero_if_0( \(x), \(p))\n") /\
%     if card(dom(x))<20 then
%       let { array[int] of var int: pp = eq_encode(x) } in
%       my_trace ("    aux_int_le_zero_if_0 by unary: dom(x) = " ++ show(dom(x)) ++ "\n") /\
%       sum ( i in dom(x) where i<=0 ) ( pp[i] ) == p
%     else
  if fMZN__UseIndicators then
    my_trace__DBG("  aux_int_le_zero_if_0  INDIC: \(x) \\/ \(p) \n") /\
    aux_int_le_zero_if_0__IND(x, p)
  else
      x <= ub(x) * p
  endif;

predicate aux_float_le_zero_if_0(var float: x, var int: p) =
  if fMZN__UseIndicators then
    my_trace__DBG("  aux_float_le_zero_if_0  INDIC: \(x) \\/ \(p)  \n") /\
    aux_float_le_zero_if_0__IND(x, p)
  else
    x <= ub(x) * p
  endif;

predicate aux_float_lt_zero_if_0(var float: x, var int: p) =
    let { float: rho = 1e-02 * abs(ub(x)) }  % same order of magnitude as ub(x)
    in
    x < (ub(x) + rho) * p;


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

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

annotation bool_search(array[int] of var bool: x, ann:a1, ann:a2, ann:a3) =
  int_search([bool2int(x[i]) | i in index_set(x)],a1,a2,a3);


predicate array_float_minimum_I(var float: m, array[int] of var float: x) =
    let { int: n = length(x),
          constraint assert(1 == min(index_set(x)), "  array_float_minimum_I: argument indexed not from 1??"),
          array[1..n] of var 0..1: p,
          int: iMinUB = arg_min([ub(x[i]) | i in 1..n]),
          float: MinUB = ub(x[iMinUB]),
          set of int: sLBLess = { i | i in 1..n where lb(x[i])<MinUB },
          set of int: sUBEqual = { i | i in 1..n where ub(x[i])==MinUB },
          set of int: sActive = if card(sLBLess intersect sUBEqual)>0 then sLBLess
                                else sLBLess union { iMinUB } endif,
          constraint     m >= lb_array(x) /\   m <= MinUB,
          array[int] of float: AL = [ lb(x[i]) | i in 1..n],
          array[int] of int: srt = sort_by([i | i in 1..n], AL),
                                            %indices of lb in sorted order
          array[int] of float: AL_srt = [AL[srt[i]] | i in 1..n],
          array[int] of float: AU_srt = [ub(x[srt[i]]) | i in 1..n],
          array[int] of float: AM_srt = AL_srt ++ [MinUB]
                                        %% -- these are z-levels of extreme points
        } in
%    assert(index_set(x) == index_set(AL),
%    "array_float_minimum_I: second argument must have index set 1..length()") /\
       my_trace("array_float_minimum_I: " ++ show(lb(m)) ++ " <= m <= " ++ show(ub(m)) 
        ++ "\n AL = " ++ show(AL)
        ++ "\n srt_lb = " ++ show(srt)
        ++ "\n AL_srt = " ++ show(AL_srt)
        ++ "\n AU_srt = " ++ show(AU_srt)
        ++ "\n MinUB = " ++ show(MinUB)
        ++ "\n sActive = " ++ show(sActive)
        ++ "\n" ) /\
    1 == sum(p) /\
    forall (i in index_set(x))
      ( 
        if i in sActive                               %% for at least 1 element
        then 
             m<=x[i] /\ aux_float_ge_if_1(m, x[i], p[i])
        else 0==p[i] endif )
                                                      %% -- exclude too big x[i]
    /\
      forall (i in 2..n+1  where
        AM_srt[i]<=MinUB  /\                          %% this is a new "start level"
        AM_srt[i]!=AM_srt[i-1]    )(                  %% and would produce a new cut
      m >= AM_srt[i]
         - sum(j in 1..i-1 where AL_srt[j]<AM_srt[i] /\ AL_srt[j]<AU_srt[j])
           ( (AU_srt[j]-x[srt[j]]) * (AM_srt[i]-AL_srt[j]) / (AU_srt[j]-AL_srt[j]) )  :: MIP_cut
                                   )
    /\
    if card(sActive)>1 /\ fElementCuts then
           array_var_float_element__XBZ_lb([ -x[i] | i in sActive ],  [  p[i] | i in sActive ], -m)  :: MIP_cut 
    else true
    endif
  ;



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% LOGICAL CONSTRAINTS TO THE SOLVER %%%%%%%%%%%%%%%%%%%%%%%%%%
predicate int_lin_eq_reif__IND(array[int] of int: c, array[int] of var int: x, int: d, var bool: b);
predicate int_lin_le_reif__IND(array[int] of int: c, array[int] of var int: x, int: d, var bool: b);
predicate int_lin_ne__IND(array[int] of int: c, array[int] of var int: x, int: d);
predicate aux_int_le_zero_if_0__IND(var int: x, var int: b);
predicate float_lin_le_reif__IND(array[int] of float: c, array[int] of var float: x, float: d, var bool: b);
predicate aux_float_eq_if_1__IND(var float: x, var float: y, var int: b);
predicate aux_float_le_zero_if_0__IND(var float: x, var int: b);

predicate array_int_minimum__IND(var int: m, array[int] of var int: x);
predicate array_int_maximum__IND(var int: m, array[int] of var int: x);
predicate array_float_minimum__IND(var float: m, array[int] of var float: x);
predicate array_float_maximum__IND(var float: m, array[int] of var float: x);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% XBZ cut generator, currently CPLEX only %%%%%%%%%%%%%%%%%%%%%%%%%%
predicate array_var_float_element__XBZ_lb__cutgen(array[int] of var float: x, array[int] of var int: b, var float: z);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Necessary for MIP: %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mzn_opt_only_range_domains = true;

