/*
% FlatZinc built-in redefinitions for linear solvers.
%
% AUTHORS
% Sebastian Brand
% Gleb Belov (2015)
%   cf. Belov, Tack, Wallace. Updated Linearization Library for MiniZinc 2.0. ModRef Workshop, CP 2015.
% TODO McCormick envelopes
*/
%-----------------------------------------------------------------------------%
ann: user_cut;
ann: lazy_constraint;
%%% comment away the assignments to have those as normal constraints  
ann: MIP_cut = user_cut;     %% seems better on average but wrong LB e.g. on carpet-cutting
ann: MIP_lazy; % = lazy_constraint;

%-----------------------------------------------------------------------------%
%  Parameters
bool: fMZN__UseIndicators=false;    %% Pass on indicator constraints, CPLEX only.  [DISABLED]
    %% Reifs give wrong result on 2012/amaze, so using implications only
opt bool: fMIPdomains;
bool: fPostprocessDomains =      %% True to pass all domain-related
           if absent( fMIPdomains ) then true                         %% constraints (...__POST) to FlatZinc
           else deopt( fMIPdomains ) endif;
bool: fPostproDom_AUX = fPostprocessDomains;   %% Specialized for aux_ constr

bool: fMZN__IgnoreRedundantCumulative=false;
%% NOT WORKING NOW, use redefs_2.0.2.mzn: 
%%%%% bool: fMZN__IgnoreAllUserRedundant=false;   %% ignore all user-spec redundant constr

bool: fElementCutsXZ=false;       %% Use simple XZ & XZB cuts for element
bool: fElementCutsXZB=false;       %% Use simple XZ & XZB cuts for element
bool: fMinimumCutsXZ=false;       %% Use simple XZ & XZB cuts for minimum
bool: fMinimumCutsXZB=false;       %% Use simple XZ & XZB cuts for minimum
bool: fUseXBZCutGen=false;      %% Use XZB cut generator for element & minimum

bool: fIntTimesBool=true;       %% Special handling of multiplication with a boolean(*const)

int: nMZN__UnarySizeMax_intTimes=20;
int: nMZN__UnarySizeMax_cumul=2000;
int: nMZN__UnarySizeMax_1step_regular=20000;   %% network-flow decomp in the regular constraint

%-----------------------------------------------------------------------------%
%    If not postprocessing domains: For unary encoding: maximal domain length to invoke it

int: nMZN__UnaryLenMin__ALL=1;    %% can be used by the indiv. cases
int: nMZN__UnaryLenMax__ALL=2000;    %% can be used by the indiv. cases
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
% The relative epsilon
float: float_lt_EPS_coef__ = 1e-03;

% Uncomment the following redefinition for FlatZinc MIP solver interfaces that
% do not support strict inequality.  Note that it does not preserve equivalence
% (some solutions of the original problem may become invalid).

% predicate float_lt(var float: x, var float: y)
    % = (x - y) <= (1.0+float_lt_EPS_coef__)*ub(x - y);

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

%---------------------------- BASIC (HALF)REIFS -----------------------------%

include "redefs_bool_reifs.mzn";
    
include "domain_encodings.mzn";
include "redefs_lin_reifs.mzn";
include "redefs_lin_halfreifs.mzn";

include "lex_less.mzn";         %% For set_le, set_lt ...
include "lex_lesseq.mzn";    
    
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

predicate array_float_minimum_I(var float: m, array[int] of var float: x) =
    let { int: n = length(x),
          constraint assert(1 == min(index_set(x)), "  array_float_minimum_I: argument indexed not from 1??"),
          int: iMinUB = arg_min([ub(x[i]) | i in 1..n]),
          float: MinUB = ub(x[iMinUB]),
          set of int: sLBLess = { i | i in 1..n where lb(x[i])<MinUB },
          set of int: sUBEqual = { i | i in 1..n where ub(x[i])==MinUB },
          set of int: sActive = if card(sLBLess intersect sUBEqual)>0 then sLBLess
                                else sLBLess union { iMinUB } endif,
    } in
    if 1==card(sActive) then
      m == x[min(sActive)]
    else
      let {
          array[1..n] of var 0..1: p,
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
      if card(sActive)>1 /\ fMinimumCutsXZ then
        forall (i in 2..n+1  where
          AM_srt[i]<=MinUB  /\                          %% this is a new "start level"
          AM_srt[i]!=AM_srt[i-1]    )(                  %% and would produce a new cut
        m >= AM_srt[i]
          - sum(j in 1..i-1 where AL_srt[j]<AM_srt[i] /\ AL_srt[j]<AU_srt[j])
            ( (AU_srt[j]-x[srt[j]]) * (AM_srt[i]-AL_srt[j]) / (AU_srt[j]-AL_srt[j]) )  :: MIP_cut
                                    )
      else true endif
      /\
      if card(sActive)>1 /\ fMinimumCutsXZB then
            array_var_float_element__XBZ_lb([ -x[i] | i in sActive ],  [  p[i] | i in sActive ], -m)  :: MIP_cut 
      else true endif
    endif
  ;

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
  elseif card(dom(x)) * card(dom(y)) > nMZN__UnarySizeMax_intTimes
    \/ ( fIntTimesBool /\ (
          %% Peter's idea for *bool. More optimal but worse values on carpet cutting.
          (card(dom(x))==2 /\ 0 in dom(x))
          \/ (card(dom(y))==2 /\ 0 in dom(y))
        ) )
    then                    %% PARAM
    %% ALSO NO POINT IF <=4.          TODO
    if card(dom(x)) > card(dom(y)) \/
      ( card(dom(x))==card(dom(y)) /\ 0 in dom(y) /\ not (0 in dom(x)) )
      then int_times(y,x,z)
    else
       my_trace ( " int_times(x,y,z): dom(x) = " ++ show(dom(x))
         ++ ", dom(y) = " ++ show(dom(y)) ++ "\n" ) /\
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
                ++ show(dom(i00))
            % "\n  LB(a) = " ++ show([lb(a[i]) | i in ix]) 
            % "\n  UB(a) = " ++ show([ub(a[i]) | i in ix])
            % ",  z in " ++ show(lb(z)) ++ ".." ++ show(ub(z))
            ++ "\n") /\
      assert(dom(i00) subset index_set(p), "", true) /\
  %%% The convexified bounds seem slow for ^2 and ^3 equations:
  %    sum(i in dom(i01))( lb(a[i]) * int2float(p[i]) ) <= z /\    %% convexify lower bounds
  %    sum(i in dom(i01))( ub(a[i]) * int2float(p[i]) ) >= z /\    %% convexify upper bounds
      forall (i in dom(i00))(
          aux_float_eq_if_1(z, a[i], p[i])
      )
      %% Cuts:
        /\ 
        if fElementCutsXZ then
          array_var_float_element__ROOF([  a[i] | i in dom(i00) ],  z)  :: MIP_cut   %% these 2 better as user cuts - too slow
          /\ array_var_float_element__ROOF([ -a[i] | i in dom(i00) ], -z)  :: MIP_cut   %% or even skip them
        else true endif
        /\
        if fElementCutsXZB then
              array_var_float_element__XBZ_lb([  a[i] | i in dom(i00) ],  [  p[i] | i in dom(i00) ],  z)  :: MIP_cut
          /\    array_var_float_element__XBZ_lb([ -a[i] | i in dom(i00) ],  [  p[i] | i in dom(i00) ], -z)  :: MIP_cut
        else true endif
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
    let {
      int: i1 = min(index_set(x))
    } in
    (z <= sum(i in index_set(x))(ub(x[i]) * b[i])) %:: MIP_cut   -- does not work to put them here TODO
    /\
    forall(i in index_set(x) intersect i1..(i1+19))   %% otherwise too many on amaze2
      ( assert(lb(x[i]) == -ub(-x[i]) /\ ub(x[i]) == -lb(-x[i]), " negated var's bounds should swap " ) /\
        z <= x[i] + sum(j in index_set(x) where i!=j)((ub(x[j])-lb(x[i]))*b[j])) %:: MIP_cut   %% (ub_j-lb_i) * b_j
    /\
    forall(i in index_set(x) intersect i1..(i1+19))
      ( z <= ub(x[i])*b[i] + sum(j in index_set(x) where i!=j)(x[j]+lb(x[j])*(b[j]-1)) ) %:: MIP_cut
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

%% ----------------------------------------------- (NO) SETS ----------------------------------------------
% XXX  only for a fixed set here,  general see below:

predicate set_in(var int: x, set of int: s__) =
    my_trace("  set_in ( x: " ++ if has_bounds(x) then show(dom(x)) else "int" endif ++ ", s__: " ++ show(s__) ++ " )\n" ) /\
    let {
      set of int: s = if has_bounds(x) then s__ intersect dom(x) else s__ endif,
      constraint  min(s) <= x /\ x <= max(s);
    } in
    if s = min(s)..max(s) then true
    elseif fPostprocessDomains then
      set_in__POST(x, s)
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
  if is_fixed(b) then
    if true==fix(b) then x in s__ else x in dom(x) diff s__ endif
  else
    my_trace("  set_in_reif ( x: " ++ if has_bounds(x) then show(dom(x)) else "int" endif ++ ", s__: " ++ show(s__) ++ " )\n" ) /\
    let {
      set of int: s = if has_bounds(x) then s__ intersect dom(x) else s__ endif,
    } in
    (
         if dom(x) subset s then b==true
         elseif card(dom(x) intersect s)==0 then b==false
         elseif s = min(s)..max(s) then
             b <-> (min(s) <= x /\ x <= max(s))
         elseif fPostprocessDomains then
           set_in_reif__POST(x, s, b)
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
    )
  endif;

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
    array[min(ub(x))..max(ub(x))] of var bool: b =
      [ set2bools_bit( x, i ) | i in min(ub(x))..max(ub(x)) ];
    constraint (x = reverse_map(b)) :: is_reverse_map;
    constraint   my_trace("  set2bools(\(x)) = \(b) \n")
  } in b
  endif;
array[int] of var bool: set2bools(var set of int: x, set of int: ubx) ::promise_total =
  if is_fixed(x) then set2bools(fix(x), ubx) else
  let {
    array[int] of var bool: b0 = set2bools( x );       %% Call in any case ?? TODO
    array[min(ubx)..max(ubx)] of var bool: b =
      [ if i in ubx then set2bools_bit( x, i ) else false endif
        | i in min(ubx)..max(ubx) ];
%% Not again:    constraint (x = reverse_map(b)) :: is_reverse_map;
    constraint   my_trace("  set2bools(\(x), \(ubx)) = \(b) \n")
  } in b
  endif;
array[int] of bool: set2bools(set of int: x) ::promise_total =
  array1d(min(x)..max(x),[i in x | i in min(x)..max(x)]);
array[int] of bool: set2bools(set of int: x, set of int: ubx) ::promise_total =
  array1d(min(ubx)..max(ubx),[i in x | i in min(ubx)..max(ubx)]);
function var bool: set2bools_bit( var set of int: x, int: i ) ::promise_total =
  if i in ub(x) then
  let {
    var bool: bi;
  } in bi
  else false endif;
  
  
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
  if is_fixed(b) then
    if true==fix(b) then x==y else x!=y endif
  else
    let {
      array[int] of var bool: bx = set2bools(x);
      array[int] of var bool: by = set2bools(y);
    } in b <-> forall (i in index_set(bx) union index_set(by)) (
      if not (i in index_set(bx)) then not by[i]
      elseif not (i in index_set(by)) then not bx[i]
      else bx[i]=by[i] endif
    )
  endif;

predicate set_ne(var set of int: x, var set of int: y) =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
  } in exists (i in index_set(bx) union index_set(by)) (
    if not (i in index_set(bx)) then by[i]
    elseif not (i in index_set(by)) then bx[i]
    else bx[i]!=by[i] endif
  );

predicate set_le(var set of int: x, var set of int: y) ::promise_total =
  if not has_ub_set(x) \/ not has_ub_set(y) then
    assert(false, "set_le: cannot determine bounds of set variables")
  else
  let {
    set of int: ubxy = ub(x) union ub(y);
    array[int] of var bool: bx = set2bools(x, ubxy);
    array[int] of var bool: by = set2bools(y, ubxy);
  } in
%%    trace( show(ub(x)) ++ ", " ++ show(ub(y)) ++ " = " ++ show(ubxy) ++ "\n") /\
    lex_lesseq(by, bx)              %% Swap order
  endif;

predicate set_le_reif(var set of int: x, var set of int: y, var bool: p) ::promise_total =
  if not has_ub_set(x) \/ not has_ub_set(y) then
    assert(false, "set_le_REIF: cannot determine bounds of set variables")
  else
  let {
    set of int: ubxy = ub(x) union ub(y);
    array[int] of var bool: bx = set2bools(x, ubxy);
    array[int] of var bool: by = set2bools(y, ubxy);
  } in
    lex_lesseq_bool_reif(by, bx, p)
  endif;

predicate set_lt(var set of int: x, var set of int: y) ::promise_total =
  if not has_ub_set(x) \/ not has_ub_set(y) then
    assert(false, "set_lt: cannot determine bounds of set variables")
  else
  let {
    set of int: ubxy = ub(x) union ub(y);
    array[int] of var bool: bx = set2bools(x, ubxy);
    array[int] of var bool: by = set2bools(y, ubxy);
  } in
    lex_less(by, bx)
  endif;

predicate set_lt_reif(var set of int: x, var set of int: y, var bool: p) ::promise_total =
  if not has_ub_set(x) \/ not has_ub_set(y) then
    assert(false, "set_lt_reif: cannot determine bounds of set variables")
  else
  let {
    set of int: ubxy = ub(x) union ub(y);
    array[int] of var bool: bx = set2bools(x, ubxy);
    array[int] of var bool: by = set2bools(y, ubxy);
  } in
    lex_less_bool_reif(by, bx, p)
  endif;

predicate set_subset(var set of int: x, var set of int: y) =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
  } in forall (i in index_set(bx)) (
    if not (i in index_set(by)) then not bx[i]
    else bx[i] -> by[i] endif
  );

predicate set_subset_reif(var set of int: x, var set of int: y, var bool: b) =
%  if is_fixed(b) then                           TODO
%    if true==fix(b) then x subset y else x!=y endif
%  else
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
%-----------------------------------------------------------------------------%

annotation bool_search(array[int] of var bool: x, ann:a1, ann:a2, ann:a3) =
  int_search([bool2int(x[i]) | i in index_set(x)],a1,a2,a3);




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DOMAIN POSTPROCESSING BUILT-INS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Single variable:  x = d  <->  x_eq_d[d]
predicate equality_encoding__POST(var int: x, array[int] of var int: x_eq_d);

%%%%%%% var int: b:  bool2int is a reverse_map, not passed to .fzn
predicate set_in__POST(var int: x, set of int: s__);
predicate set_in_reif__POST(var int: x, set of int: s__, var int: b);




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% LOGICAL CONSTRAINTS TO THE SOLVER %%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%% var int: b:  bool2int is a reverse_map, not passed to .fzn  =>  REPEAT TESTS. TODO
predicate int_lin_eq_reif__IND(array[int] of int: c, array[int] of var int: x, int: d, var int: b);
predicate int_lin_le_reif__IND(array[int] of int: c, array[int] of var int: x, int: d, var int: b);
predicate int_lin_ne__IND(array[int] of int: c, array[int] of var int: x, int: d);
predicate aux_int_le_zero_if_0__IND(var int: x, var int: b);
predicate float_lin_le_reif__IND(array[int] of float: c, array[int] of var float: x, float: d, var int: b);
predicate aux_float_eq_if_1__IND(var float: x, var float: y, var int: b);
predicate aux_float_le_zero_if_0__IND(var float: x, var int: b);

predicate array_int_minimum__IND(var int: m, array[int] of var int: x);
predicate array_int_maximum__IND(var int: m, array[int] of var int: x);
predicate array_float_minimum__IND(var float: m, array[int] of var float: x);
predicate array_float_maximum__IND(var float: m, array[int] of var float: x);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% XBZ cut generator, currently CPLEX only %%%%%%%%%%%%%%%%%%%%%%%%%%
predicate array_var_float_element__XBZ_lb__cutgen(array[int] of var float: x, array[int] of var int: b, var float: z);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Necessary for MIP: %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mzn_opt_only_range_domains = not fPostprocessDomains;

