/*
% FlatZinc built-in redefinitions for linear solvers.
%
% AUTHORS
% Sebastian Brand
% Gleb Belov
*/

%-----------------------------------------------------------------------------%
%
% Linear equations and inequations
% TODO Use indicators for (half)reifs.
% Otherwise and more using unary encoding for reasonable domains
%
%-----------------------------------------------------------------------------%

% Domains: reduce all to aux_ stuff?
%% never use Concert's reif feature

%% var, var
predicate int_le_reif(var int: x, var int: y, var bool: b) =
%%  trace(" int_le_reif VV: \(x), \(y), \(b) \n") /\
  if is_fixed(b) then
    if true==fix(b) then x<=y else x>y endif
  elseif ub(x)<=lb(y) then b==true
  elseif lb(x)>ub(y) then b==false
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    int_le_reif__POST(x-y, 0, b)
  else
    int_le_reif__NOPOST(x, y, b)
  endif
;

%% const, var
predicate int_le_reif(int: x, var int: y, var bool: b) =
%%  trace(" int_le_reif CV: \(x), \(y), \(b) \n") /\
  if is_fixed(b) then
    if true==fix(b) then x<=y else x>y endif
  elseif ub(x)<=lb(y) then b==true
  elseif lb(x)>ub(y) then b==false
  elseif fPostprocessDomains then
    int_ge_reif__POST(y, x, b)
  else
    int_le_reif(-y, -x, b)
  endif
;

%% var, const
predicate int_le_reif(var int: x, int: y, var bool: b) =
%%  trace(" int_le_reif VC: \(x), \(y), \(b) \n") /\
  if is_fixed(b) then
    if true==fix(b) then x<=y else x>y endif
  elseif ub(x)<=lb(y) then b==true
  elseif lb(x)>ub(y) then b==false
  else
    if fPostprocessDomains then
      int_le_reif__POST(x, y, b)
    else
      int_le_reif__NOPOST(x, y, b)
    endif
  endif
;

%% var int, var int
predicate int_le_reif__NOPOST(var int: x, var int: y, var bool: b) =
        aux_int_le_if_1(x, y, b) /\            %% This can call POSTs...  TODO
        aux_int_gt_if_0(x, y, b)
      ;

%% var, var
predicate int_lt_reif(var int: x, var int: y, var bool: b) =
    %% int_le_reif(x-y, -1, b);   %% This would produce a new variable x-y and possibly POST it
                                  %% but it looks like differences should not be
    if is_fixed( x ) then
      int_le_reif(x+1, y, b)
    else
      int_le_reif(x, y-1, b)
    endif;

%% var, var
predicate int_ne(var int: x, var int: y) =
    if fPostproDom_DIFF then
      int_ne(x-y, 0)
    else
      int_ne__SIMPLE(x-y, 0)
    endif;

%% var, const
predicate int_ne(var int: x, int: y) =
     if y in dom(x) then 
       if y==ub(x) then
         x <= y-1
       elseif y==lb(x) then
         x >= y+1
       elseif fPostprocessDomains then
         int_ne__POST(x, y)
       elseif card(dom(x))<nMZN__UnaryLenMax_neq then
          let {
            array[int] of var int: p = eq_encode(x);
          } in
          p[y]==false
       else int_ne__SIMPLE(x, y)
       endif
     else
       true
     endif;

%% var, const. No postprocessing
predicate int_ne__SIMPLE(var int: x, int: y) =
  if false then
    true
  else
    let { var 0..1: p }
    in
    aux_int_lt_if_1(x, y, p) /\
    aux_int_gt_if_0(x, y, p)
  endif;

%% var, var
predicate int_eq_reif(var int: x, var int: y, var bool: b) =
%%    trace(" int_eq_reif VV: \(x), \(y), \(b) \n") /\
    if is_fixed(b) then
      if true==fix(b) then x==y else x!=y endif
    elseif card(dom(x) intersect dom(y))>0 then
      if is_fixed(x) then
        if is_fixed(y) then
          b <-> fix(x)==fix(y)
        else
          int_eq_reif(y, fix(x), b)
        endif
      elseif is_fixed(y) then
        int_eq_reif(x, fix(y), b)
      elseif fPostprocessDomains /\ fPostproDom_DIFF then
        int_eq_reif(x-y, 0, b)
      else
        aux_int_eq_iff_1(x, y, b)
      endif
    else
      not b
    endif;

%% var, const
predicate int_eq_reif(var int: x, int: y, var bool: b) =
%%  trace(" int_eq_reif VC: \(x), \(y), \(b) \n") /\
  if is_fixed(b) then
    if true==fix(b) then x==y else x!=y endif
  elseif y in dom(x) then
    if is_fixed(x) then
      b <-> y==fix(x)
    elseif fPostprocessDomains then
      int_eq_reif__POST(x, y, b)
%%%  THIS seems pretty complex, especially for binaries, and does not connect to eq_encoding (/ MIPD?)
%%    elseif y==lb(x) then
%%      int_lt_reif(y, x, not b)
%%    elseif y==ub(x) then
%%      int_lt_reif(x, y, not b)
    elseif card(dom(x))<nMZN__UnaryLenMax_eq then
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

%% To avoid creating a new int var = sum...
function var float: sum2float( array[int] of int: c, array[int] of var int: x ) =
  let {
    array[int] of float: cF = c;
    array[int] of var float: xF = x;
  } in
    sum(i in index_set(xF))( cF[i]*xF[i] );

%% lin_expr, const
predicate int_lin_le_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
  if fPostprocessDomains /\ fPostproDom_DIFF then
    int_le_reif(sum(i in index_set(x))( c[i] * x[i] ), d, b)
  elseif fAvoidNI then
    aux_float_le_if_1(sum2float( c, x ), d, b) /\
    aux_float_ge_if_0(sum2float( c, x ), d+1, b)
  else
    int_le_reif__NOPOST(sum(i in index_set(x))( c[i] * x[i] ), d, b)
  endif;


predicate int_lin_lt_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
  if true then
    abort("int_lin_lt_reif not supposed to be called")
  else
    int_lin_le_reif(c, x, d - 1, b)
  endif;

%% lin_expr, const
predicate int_lin_eq_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
    if fPostprocessDomains /\ fPostproDom_DIFF then
      int_eq_reif(sum(i in index_set(x))( c[i]*x[i] ), d, b)
    elseif fAvoidNI then
      aux_int_eq_iff_1__float(sum2float( c, x ), d, b)
    else
      aux_int_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d, b)
    endif;

%% lin_expr, const
predicate int_lin_ne(array[int] of int: c, array[int] of var int: x, int: d) =
  if fPostprocessDomains /\ fPostproDom_DIFF then
    int_ne(sum(i in index_set(x))( c[i]*x[i] ),d)
  elseif fAvoidNI then
    int_lin_eq_reif(c, x, d, false)
  else
    int_ne__SIMPLE(sum(i in index_set(x))( c[i]*x[i] ),d)
  endif;

%% lin_expr, const
predicate int_lin_ne_reif(array[int] of int: c, array[int] of var int: x,
                          int: d, var bool: b) =
    if fPostprocessDomains /\ fPostproDom_DIFF then
      int_ne_reif(sum(i in index_set(x))( c[i]*x[i] ), d, b)
    else
      int_lin_eq_reif(c, x, d, not (b))
    endif;


%-----------------------------------------------------------------------------%

%% var float, const
predicate float_le_reif(var float: x, float: y, var bool: b) =
%%  trace( "float_le_reif(\(x), \(y), \(b))\n" ) /\
  if is_fixed(b) then
    if true==fix(b) then x<=y else x>y endif
  elseif false then
    float_lin_le_reif__IND( [1.0, -1.0], [x, y], 0.0, b)
  elseif ub(x) <= y then
    b == true
  elseif lb(x) > y then
    b == false
  elseif fPostprocessDomains then
    float_le_reif__POST(x, y, b, float_lt_EPS)
  else
    float_le_reif__NOPOST(x, y, b)
  endif;

%% const, var float
predicate float_le_reif(float: x, var float: y, var bool: b) =
  if is_fixed(b) then
    if true==fix(b) then x<=y else x>y endif
  elseif false then
    float_lin_le_reif__IND( [1.0, -1.0], [x, y], 0.0, b)
  elseif ub(x) <= lb(y) then
    b == true
  elseif lb(x) > ub(y) then
    b == false
  elseif fPostprocessDomains then
    float_ge_reif__POST(y, x, b, float_lt_EPS)
  else
    float_le_reif(-y, -x, b)
  endif;

%% var float, var float
predicate float_le_reif(var float: x, var float: y, var bool: b) =
  if is_fixed(b) then
    if true==fix(b) then x<=y else x>y endif
  elseif ub(x)<=lb(y) then b==true
  elseif lb(x)>ub(y) then b==false
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    float_le_reif(x-y, 0.0, b)
  else
    float_le_reif__NOPOST(x-y, 0, b)
  endif
;

%% var float, var float
predicate float_le_reif__NOPOST(var float: x, var float: y, var bool: b) =
    aux_float_le_if_1(x, y, (b)) /\      %% Can call __POSTs     TODO
    aux_float_gt_if_0(x, y, (b))
    ;

%% TODO
predicate float_lt_reif(var float: x, var float: y, var bool: b) =
  %% Actually = float_le_reif(x, y-eps, b).
  if is_fixed(b) then
    if true==fix(b) then x<y else x>=y endif
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    aux_float_lt_zero_iff_1__POST(x - y, b, float_lt_EPS)
  else
    aux_float_lt_if_1(x, y, (b)) /\
    aux_float_ge_if_0(x, y, (b))
  endif;

%% var, const
predicate float_ne(var float: x, float: y) =
  if fPostprocessDomains then
    float_ne__POST(x, y, float_lt_EPS)
  else
    float_ne__SIMPLE(x, y)
  endif;

predicate float_ne__SIMPLE(var float: x, var float: y) =
    let { var 0..1: p }
    in
    aux_float_lt_if_1(x, y, (p)) /\
    aux_float_gt_if_0(x, y, (p))
    ;

%% var, var
predicate float_ne(var float: x, var float: y) =
  if fPostprocessDomains /\ fPostproDom_DIFF then
    float_ne(x-y, 0.0)
  else
    float_ne__SIMPLE(x-y, 0.0)
  endif;

%% TODO Why is disequality with EPS but equality not??
    
%% var, var? TODO
predicate float_eq_reif(var float: x, var float: y, var bool: b) =
  if is_fixed(b) then
    if true==fix(b) then x==y else x!=y endif
  elseif ub(x)<lb(y) \/ lb(x)>ub(y) then
    b == false
  elseif is_fixed(x) /\ is_fixed(y) then
    b == (fix(x) == fix(y))
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    float_eq_reif__POST(x-y, 0, b, float_lt_EPS)
  else
    aux_float_eq_iff_1(x, y, (bool2int(b)))
  endif;

predicate float_ne_reif(var float: x, var float: y, var bool: b) =
    float_eq_reif(x, y, not (b));

%-----------------------------------------------------------------------------%

predicate float_lin_eq_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
  if fPostprocessDomains /\ fPostproDom_DIFF then
    float_eq_reif(sum(i in index_set(x))( c[i]*x[i] ), d, b)
  else
    aux_float_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d, b)
  endif;

predicate float_lin_ne_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
  if fPostprocessDomains /\ fPostproDom_DIFF then
    float_ne_reif(sum(i in index_set(x))( c[i]*x[i] ), d, not b)
  else
    aux_float_eq_iff_1(sum(i in index_set(x))( c[i]*x[i] ), d, not b)
  endif;

predicate float_lin_le_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
  if fPostprocessDomains /\ fPostproDom_DIFF then
    float_le_reif(sum(i in index_set(x))( c[i]*x[i] ), d, b)
  else
    float_le_reif__NOPOST(sum(i in index_set(x))( c[i]*x[i] ), d, b)
  endif;

predicate float_lin_lt_reif(array[int] of float: c, array[int] of var float: x,
                            float: d, var bool: b) =
    float_lt_reif(sum(i in index_set(x))( c[i]*x[i] ), d, b);


%-----------------------------------------------------------------------------%
% Auxiliary: equality reified onto a 0/1 variable

predicate aux_int_eq_iff_1(var int: x, var int: y, var int: p) =
  if is_fixed(p) then
    if 1==fix(p) then x==y else x!=y endif
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    abort("  aux_int_eq_iff_1 should not be used with full domain postprocessing")
  elseif false then
    true
  else
    let { array[1..2] of var 0..1: q }
    in
    aux_int_le_if_1(x, y, p) /\
    aux_int_ge_if_1(x, y, p) /\
    aux_int_lt_if_0(x, y, q[1]) /\
    aux_int_gt_if_0(x, y, q[2]) /\
    sum(q) == p + 1
  endif;

predicate aux_int_eq_iff_1__float(var float: x, float: y, var int: p) =
    let { array[1..2] of var 0..1: q }
    in
    aux_int_le_if_1(x, y, p) /\
    aux_int_ge_if_1(x, y, p) /\
    aux_int_lt_if_0(x, y, q[1]) /\
    aux_int_gt_if_0(x, y, q[2]) /\
    sum(q) == p + 1
    ;

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
  if is_fixed(p) then
    if 1==fix(p) then x==y else x!=y endif
  elseif fPostprocessDomains /\ fPostproDom_DIFF then
    abort("  aux_float_eq_iff_1 should not be used with full domain postprocessing")
  else
    let { array[1..2] of var 0..1: q }
    in
    aux_float_le_if_1(x, y, p) /\
    aux_float_ge_if_1(x, y, p) /\
    aux_float_lt_if_0(x, y, (q[1])) /\
    aux_float_gt_if_0(x, y, (q[2])) /\
    sum(i in 1..2)((q[i])) == 1 + p
  endif;

  
  
% ----------------------------- Domains postpro ---------------------------
    
%%%%%%%%%%%%%%%%%% predicate int_le_reif__POST(var int: x, var int: y, var int: b);
%%%%%%%%%%%%%%%%%% predicate int_le_reif__POST(int: x, var int: y, var int: b);
%%%%%%% var int: b:  bool2int is a reverse_map, not passed to .fzn
%% var, const
predicate int_le_reif__POST(var int: x, int: y, var int: b);
%% var, const
predicate int_ge_reif__POST(var int: x, int: y, var int: b);

%% var, const
predicate int_eq_reif__POST(var int: x, int: y, var int: b);
%% var, const
predicate int_ne__POST(var int: x, int: y);

%%%%%%%%%%%%%%%%%% predicate float_le_reif__POST(var float: x, var float: y, var int: b);
%%%%%%%%%%%%%%%%%% predicate float_le_reif__POST(float: x, var float: y, var int: b);
%% var, const
predicate float_le_reif__POST(var float: x, float: y, var int: b, float: epsRel);
%% var, const
predicate float_ge_reif__POST(var float: x, float: y, var int: b, float: epsRel);

%% var, var
predicate aux_float_lt_zero_iff_1__POST(var float: x, var int: b, float: epsRel);

%% var, const
predicate float_eq_reif__POST(var float: x, float: y, var int: b, float: epsRel);
%% var, const
predicate float_ne__POST(var float: x, float: y, float: epsRel);


