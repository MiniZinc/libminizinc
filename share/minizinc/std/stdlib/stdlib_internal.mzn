%-----------------------------------------------------------------------------%
%
% Internal compiler functions
%
% These functions are used internally by the compiler.
%

% domain constraints
predicate var_dom(var int:x, set of int: s) =
  if has_bounds(x) /\ dom(x) subset s then true
  else x in s
  endif;
predicate var_dom(var opt int:x, set of int: s) =
  let {
    var int: dx = deopt(x);
    set of int: new_dom = dom(dx) intersect s;
  } in if new_dom = {} then absent(x) else dx in new_dom endif;
predicate var_dom(array[$T] of var opt int: x, set of int: d) =
    let { array[int] of var opt int: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));

predicate var_dom(var set of int: x, set of int: s) =
  if has_ub_set(x) /\ ub(x) subset s then true
  else set_subset(x,s)
  endif;

predicate var_dom(var float:x, float: l, float: u) =
  if has_bounds(x) /\ lb(x) >= l /\ ub(x) <= u then true
  else x >= l /\ x <= u
  endif;

predicate var_dom(var float:x, set of float: d) =
  x in d;

test var_dom(float:x, float: l, float: u) =
  x >= l /\ x <= u;

test var_dom(float:x, set of float: d) =
  x in d;

predicate var_dom(array[$T] of var set of int: x, set of int: d) =
    let { array[int] of var set of int: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));
predicate var_dom(array[$T] of var int: x, set of int: d) =
    let { array[int] of var int: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));
predicate var_dom(array[$T] of var float: x, float: l, float: u) =
    let { array[int] of var float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],l,u));
predicate var_dom(array[$T] of var float: x, set of float: d) =
    let { array[int] of var float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));

test var_dom(array[$T] of set of int: x, set of int: d) =
    let { array[int] of set of int: xx = array1d(x) }
    in forall (i in index_set(xx)) (xx[i] subset d);
test var_dom(array[$T] of int: x, set of int: d) =
    let { array[int] of int: xx = array1d(x) }
    in forall (i in index_set(xx)) (xx[i] in d);
test var_dom(array[$T] of float: x, float: l, float: u) =
    let { array[int] of float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],l,u));
test var_dom(array[$T] of float: x, set of float: d) =
    let { array[int] of float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));

predicate set_in(array[$T] of var int: X, set of int: s) = forall(x in array1d(X)) (x in s);
predicate int_eq(array[$T] of var int: X, int: s) = forall(x in array1d(X)) (x = s);
predicate float_eq(array[$T] of var int: X, float: s) = forall(x in array1d(X)) (x = s);
predicate int_le(array[$T] of var int: X, int: s) = forall(x in array1d(X)) (x <= s);
predicate int_le(int:s, array[$T] of var int: X) = forall(x in array1d(X)) (x >= s);
predicate float_le(array[$T] of var float: X, float: s) = forall(x in array1d(X)) (x <= s);
predicate float_le(float:s, array[$T] of var float: X) = forall(x in array1d(X)) (x >= s);

predicate array_var_int_element(var int: x, array[int] of int: y, var int: z) =
  array_int_element(x,y,z);
predicate array_var_bool_element(var int: x, array[int] of bool: y, var bool: z) =
  array_bool_element(x,y,z);
predicate array_var_float_element(var int: x, array[int] of float: y, var float: z) =
  array_float_element(x,y,z);
predicate array_var_set_element(var int: x, array[int] of set of int: y, var set of int: z) =
  array_set_element(x,y,z);

predicate bool_opt_eq(var opt bool: x, var opt bool: y) =
  deopt(x)=deopt(y) /\ occurs(x)=occurs(y);
/** True iff both \a b0 and \a b1 are absent or
  both are present and have the same value. */
predicate bool_eq(var opt bool: b0, var opt bool: b1) =
     (absent(b0) /\ absent(b1))
  \/ (occurs(b0) /\ occurs(b1) /\ deopt(b0)=deopt(b1));
/** True iff \a b0 occurs and is equal to \a b1 */
predicate bool_eq(var opt bool: b0, var bool: b1) =
  occurs(b0) /\ deopt(b0)=b1;
/** True iff \a b1 occurs and is equal to \a b0 */
predicate bool_eq(var bool: b0, var opt bool: b1) =
  occurs(b1) /\ deopt(b1)=b0;

predicate bool_xor_reif(var bool: a, var bool: b, var bool: c) =
  bool_xor(a,b,c);

predicate int_opt_eq(var opt int: x, var opt int: y) =
  deopt(x) = deopt(y) /\ occurs(x) = occurs(y);
/** True iff both \a x and \a y are absent or
  both are present and have the same value. */
predicate int_eq(var opt int: x, var opt int: y) =
     (absent(x) /\ absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)=deopt(y))::maybe_partial);
/** True iff only one of \a x and \a y is absent or
  both are present and have different values. */
predicate int_ne(var opt int : x, var opt int : y) =
     (absent(x) != absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)!=deopt(y))::maybe_partial);
/** Constrains \a x \( \in \) \a S */
predicate set_in(var opt int: x, set of int: S) = if occurs(x) then deopt(x) in S endif;
/** Constrains \a x \( \in \) \a S */
predicate set_in(var opt int: x, var set of int: S) = if occurs(x) then deopt(x) in S endif;

% :NOTE: does it apply to float?

/*
predicate var_dom(var opt float:x, set of float: s) =
  let {
    var float: dx = deopt(x);
    set of float: new_dom = dom(dx) intersect s;
  } in if new_dom = {} then absent(x) else dx in new_dom endif;

predicate var_dom(array[$T] of var opt float: x, set of float: d) =
    let { array[int] of var opt float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));
*/
predicate float_dom(var opt float: x, array[int] of float: as) =
  let {
    var bool : b = occurs_internal_float(x);
    var float : dx = deopt_internal_float(x);
    constraint (x = reverse_map_var_opt_float(b,dx)) :: is_reverse_map;
  } in float_dom(dx, as);

predicate float_opt_eq(var opt float: x, var opt float: y) =
  deopt(x) = deopt(y) /\ occurs(x) = occurs(y);
/** True iff both \a x and \a y are absent or
  both are present and have the same value. */
predicate float_eq(var opt float: x, var opt float: y) =
     (absent(x) /\ absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)=deopt(y))::maybe_partial);

/** True iff only one of \a x and \a y is absent or
  both are present and have different values. */
predicate float_ne(var opt float : x, var opt float : y) =
     (absent(x) != absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)!=deopt(y))::maybe_partial);

predicate xorall_reif(array[int] of var bool: b, var bool: c) =
  let { var bool: nc ::is_defined_var; constraint xorall([nc]++b) ::defines_var(nc); } in c = not nc;

function var int: lin_exp(array[int] of int, array[int] of var int, int);
function var float: lin_exp(array[int] of float, array[int] of var float, float);

test mzn_in_root_context(var $T);

test mzn_in_redundant_constraint();

test mzn_in_symmetry_breaking_constraint();

/* Internal function used to optimize over option type objective */
function var float: objective_deopt_(var opt float: x, bool: direction) =
  let {
    float: worst = if direction then lb(x)-1 else ub(x)+1 endif;
  } in if occurs(x) then deopt(x) else worst endif;


%-----------------------------------------------------------------------------%
%
% Element constraint implementations
%
% MiniZinc compiles element constraints using a series of intermediate
% functions that test whether the constraint is total and perform array slicing
% for multi-dimensional element constraints.
%

%%%%%%%%%%%%%%%%%%%
% Element on ints

function var int: element_t(var int: idx, array[int] of var int: x) :: promise_total =
  let {
    var dom_bounds_array(x): r ::is_defined_var;
    constraint idx in index_set(x);
    constraint array_var_int_element_nonshifted(idx,x,r) ::defines_var(r);
  } in r;

function var int: element_mt(var int: idx, array[int] of var int: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_int_element_nonshifted(idx2,x,r) ::defines_var(r);
  } in r;

function var int: element_t(var int: idx1, var int: idx2, array[int,int] of var int: x) :: promise_total =
  let {
    var dom_bounds_array(x): r ::is_defined_var;
    constraint idx1 in index_set_1of2(x);
    constraint idx2 in index_set_2of2(x);
    constraint array_var_int_element2d_nonshifted(idx1,idx2,x,r) ::defines_var(r);
  } in r;

function var int: element_mt(var int: idx1, var int: idx2, array[int,int] of var int: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    var min(index_set_1of2(x))..max(index_set_1of2(x)): idx1_2;
    var min(index_set_2of2(x))..max(index_set_2of2(x)): idx2_2;
    constraint (idx1 in index_set_1of2(x) /\ idx2 in index_set_2of2(x)) -> (idx1_2=idx1 /\ idx2_2=idx2);
    constraint (idx1 in index_set_1of2(x) /\ idx2 in index_set_2of2(x)) \/ (idx1_2=min(index_set_1of2(x)) /\ idx2_2=min(index_set_2of2(x)));
    constraint array_var_int_element2d_nonshifted(idx1_2,idx2_2,x,r) ::defines_var(r);
  } in r;

function var int: element(var int: idx, array[int] of var int: x) =
  if mzn_in_root_context(idx) then let {
    constraint idx in index_set(x)
  } in element_t(idx,x)
  elseif (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var int: element(var int: idx1, var int: idx2,
                          array[int,int] of var int: x) =
  let {
    int: dim = card(index_set_2of2(x));
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_t(idx1, idx2, x)
       elseif
       ((has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
        /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x))))
       then
         element_t(idx1,idx2,x)
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt(idx1,idx2,x)
       endif;

function var int: element(var int: idx1, var int: idx2, var int: idx3,
                          array[int,int,int] of var int: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x))))
       then
         element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       endif;

function var int: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4,
                          array[int,int,int,int] of var int: x) =
  let {
    int: dim2 = card(index_set_2of4(x));
    int: dim3 = card(index_set_3of4(x));
    int: dim4 = card(index_set_4of4(x));
    
    int: min = min(index_set_1of4(x))*dim2*dim3*dim4+
               min(index_set_2of4(x))*dim3*dim4+
               min(index_set_3of4(x))*dim4+
               min(index_set_4of4(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of4(x);
           constraint idx2 in index_set_2of4(x);
           constraint idx3 in index_set_3of4(x);
           constraint idx4 in index_set_4of4(x);
         } in element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       endif;

function var int: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4, var int: idx5,
                          array[int,int,int,int,int] of var int: x) =
  let {
    int: dim2 = card(index_set_2of5(x));
    int: dim3 = card(index_set_3of5(x));
    int: dim4 = card(index_set_4of5(x));
    int: dim5 = card(index_set_5of5(x));
    
    int: min = min(index_set_1of5(x))*dim2*dim3*dim4*dim5+
               min(index_set_2of5(x))*dim3*dim4*dim5+
               min(index_set_3of5(x))*dim4*dim5+
               min(index_set_4of5(x))*dim5+
               min(index_set_5of5(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of5(x);
           constraint idx2 in index_set_2of5(x);
           constraint idx3 in index_set_3of5(x);
           constraint idx4 in index_set_4of5(x);
           constraint idx5 in index_set_5of5(x);
         } in element_t( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of5(x)) /\ ub(idx1) <= max(index_set_1of5(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of5(x)) /\ ub(idx2) <= max(index_set_2of5(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of5(x)) /\ ub(idx3) <= max(index_set_3of5(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of5(x)) /\ ub(idx4) <= max(index_set_4of5(x)))
       /\ (has_bounds(idx5) /\ lb(idx5) >= min(index_set_5of5(x)) /\ ub(idx5) <= max(index_set_5of5(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of5(x);
         constraint idx2 in index_set_2of5(x);
         constraint idx3 in index_set_3of5(x);
         constraint idx4 in index_set_4of5(x);
         constraint idx5 in index_set_5of5(x);
       } in element_mt( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       endif;

function var int: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4, var int: idx5, var int: idx6,
                          array[int,int,int,int,int,int] of var int: x) =
  let {
    int: dim2 = card(index_set_2of6(x));
    int: dim3 = card(index_set_3of6(x));
    int: dim4 = card(index_set_4of6(x));
    int: dim5 = card(index_set_5of6(x));
    int: dim6 = card(index_set_6of6(x));
    
    int: min = min(index_set_1of6(x))*dim2*dim3*dim4*dim5*dim6+
               min(index_set_2of6(x))*dim3*dim4*dim5*dim6+
               min(index_set_3of6(x))*dim4*dim5*dim6+
               min(index_set_4of6(x))*dim5*dim6+
               min(index_set_5of6(x))*dim6+
               min(index_set_6of6(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of6(x);
           constraint idx2 in index_set_2of6(x);
           constraint idx3 in index_set_3of6(x);
           constraint idx4 in index_set_4of6(x);
           constraint idx5 in index_set_5of6(x);
           constraint idx6 in index_set_6of6(x);
         } in element_t( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of6(x)) /\ ub(idx1) <= max(index_set_1of6(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of6(x)) /\ ub(idx2) <= max(index_set_2of6(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of6(x)) /\ ub(idx3) <= max(index_set_3of6(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of6(x)) /\ ub(idx4) <= max(index_set_4of6(x)))
       /\ (has_bounds(idx5) /\ lb(idx5) >= min(index_set_5of6(x)) /\ ub(idx5) <= max(index_set_5of6(x)))
       /\ (has_bounds(idx6) /\ lb(idx6) >= min(index_set_6of6(x)) /\ ub(idx6) <= max(index_set_6of6(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of6(x);
         constraint idx2 in index_set_2of6(x);
         constraint idx3 in index_set_3of6(x);
         constraint idx4 in index_set_4of6(x);
         constraint idx5 in index_set_5of6(x);
         constraint idx6 in index_set_6of6(x);
       } in element_mt( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       endif;

/** Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt int: element(var opt int: idx, array[int] of var int: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt int: element(var opt int: idx1, var opt int: idx2, array[int,int] of var int: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** Return \a x[\a idx] */
function var opt int: element(var int: idx, array[int] of var opt int: x) =
  let {
    var opt int: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx, array1d(index_set(x), [if is_fixed(absent(x[i])) /\ fix(absent(x[i])) then deopt(r) else deopt(x[i]) endif | i in index_set(x)]));
  } in r;

/** Return \a x[\a idx1, \a idx2] */
function var opt int: element(var int: idx1, var int: idx2, array[int,int] of var opt int: x) =
  let {
    var opt int: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[if is_fixed(absent(x[i,j])) /\ fix(absent(x[i,j])) then deopt(r) else deopt(x[i,j]) endif | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

/** Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt int: element(var opt int: idx, array[int] of var opt int: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt int: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt int: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

%%%%%%%%%%%%%%%%%%%
% Element on floats

function var float: element_t(var int: idx, array[int] of var float: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    constraint idx in index_set(x);
    constraint array_var_float_element_nonshifted(idx,x,r) ::defines_var(r);
  } in r;

function var float: element_mt(var int: idx, array[int] of var float: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_float_element_nonshifted(idx2,x,r) ::defines_var(r);
  } in r;

function var float: element_t(var int: idx1, var int: idx2, array[int,int] of var float: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    constraint idx1 in index_set_1of2(x);
    constraint idx2 in index_set_2of2(x);
    constraint array_var_float_element2d_nonshifted(idx1,idx2,x,r) ::defines_var(r);
  } in r;

function var float: element_mt(var int: idx1, var int: idx2, array[int,int] of var float: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    var min(index_set_1of2(x))..max(index_set_1of2(x)): idx1_2;
    var min(index_set_2of2(x))..max(index_set_2of2(x)): idx2_2;
    constraint (idx1 in index_set_1of2(x) /\ idx2 in index_set_2of2(x)) -> (idx1_2=idx1 /\ idx2_2=idx2);
    constraint (idx1 in index_set_1of2(x) /\ idx2 in index_set_2of2(x)) \/ (idx1_2=min(index_set_1of2(x)) /\ idx2_2=min(index_set_2of2(x)));
    constraint array_var_float_element2d_nonshifted(idx1_2,idx2_2,x,r) ::defines_var(r);
  } in r;

function var float: element(var int: idx, array[int] of var float: x) =
  if mzn_in_root_context(idx) then let {
    constraint idx in index_set(x)
  } in element_t(idx,x)
  elseif (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var float: element(var int: idx1, var int: idx2,
                            array[int,int] of var float: x) =
  let {
    int: dim = card(index_set_2of2(x));
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_t(idx1,idx2,x)
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x))) )
       then
         element_t(idx1,idx2,x)
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt(idx1,idx2,x)
       endif;

function var float: element(var int: idx1, var int: idx2, var int: idx3,
                            array[int,int,int] of var float: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if mzn_in_root_context(idx1) then let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x))) )
       then
         element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       endif;

function var float: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4,
                            array[int,int,int,int] of var float: x) =
  let {
    int: dim2 = card(index_set_2of4(x));
    int: dim3 = card(index_set_3of4(x));
    int: dim4 = card(index_set_4of4(x));
    
    int: min = min(index_set_1of4(x))*dim2*dim3*dim4+
               min(index_set_2of4(x))*dim3*dim4+
               min(index_set_3of4(x))*dim4+
               min(index_set_4of4(x))-1;
  } in if mzn_in_root_context(idx1) then let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       endif;

function var float: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4, var int: idx5,
                            array[int,int,int,int,int] of var float: x) =
  let {
    int: dim2 = card(index_set_2of5(x));
    int: dim3 = card(index_set_3of5(x));
    int: dim4 = card(index_set_4of5(x));
    int: dim5 = card(index_set_5of5(x));
    
    int: min = min(index_set_1of5(x))*dim2*dim3*dim4*dim5+
               min(index_set_2of5(x))*dim3*dim4*dim5+
               min(index_set_3of5(x))*dim4*dim5+
               min(index_set_4of5(x))*dim5+
               min(index_set_5of5(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of5(x);
           constraint idx2 in index_set_2of5(x);
           constraint idx3 in index_set_3of5(x);
           constraint idx4 in index_set_4of5(x);
           constraint idx5 in index_set_5of5(x);
         } in element_t( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of5(x)) /\ ub(idx1) <= max(index_set_1of5(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of5(x)) /\ ub(idx2) <= max(index_set_2of5(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of5(x)) /\ ub(idx3) <= max(index_set_3of5(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of5(x)) /\ ub(idx4) <= max(index_set_4of5(x)))
       /\ (has_bounds(idx5) /\ lb(idx5) >= min(index_set_5of5(x)) /\ ub(idx5) <= max(index_set_5of5(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of5(x);
         constraint idx2 in index_set_2of5(x);
         constraint idx3 in index_set_3of5(x);
         constraint idx4 in index_set_4of5(x);
         constraint idx5 in index_set_5of5(x);
       } in element_mt( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       endif;

function var float: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4, var int: idx5, var int: idx6,
                            array[int,int,int,int,int,int] of var float: x) =
  let {
    int: dim2 = card(index_set_2of6(x));
    int: dim3 = card(index_set_3of6(x));
    int: dim4 = card(index_set_4of6(x));
    int: dim5 = card(index_set_5of6(x));
    int: dim6 = card(index_set_6of6(x));
    
    int: min = min(index_set_1of6(x))*dim2*dim3*dim4*dim5*dim6+
               min(index_set_2of6(x))*dim3*dim4*dim5*dim6+
               min(index_set_3of6(x))*dim4*dim5*dim6+
               min(index_set_4of6(x))*dim5*dim6+
               min(index_set_5of6(x))*dim6+
               min(index_set_6of6(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of6(x);
           constraint idx2 in index_set_2of6(x);
           constraint idx3 in index_set_3of6(x);
           constraint idx4 in index_set_4of6(x);
           constraint idx5 in index_set_5of6(x);
           constraint idx6 in index_set_6of6(x);
         } in element_t( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of6(x)) /\ ub(idx1) <= max(index_set_1of6(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of6(x)) /\ ub(idx2) <= max(index_set_2of6(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of6(x)) /\ ub(idx3) <= max(index_set_3of6(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of6(x)) /\ ub(idx4) <= max(index_set_4of6(x)))
       /\ (has_bounds(idx5) /\ lb(idx5) >= min(index_set_5of6(x)) /\ ub(idx5) <= max(index_set_5of6(x)))
       /\ (has_bounds(idx6) /\ lb(idx6) >= min(index_set_6of6(x)) /\ ub(idx6) <= max(index_set_6of6(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of6(x);
         constraint idx2 in index_set_2of6(x);
         constraint idx3 in index_set_3of6(x);
         constraint idx4 in index_set_4of6(x);
         constraint idx5 in index_set_5of6(x);
         constraint idx6 in index_set_6of6(x);
       } in element_mt( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       endif;

/** Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt float: element(var opt int: idx, array[int] of var float: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt float: element(var opt int: idx1, var opt int: idx2, array[int,int] of var float: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** Return \a x[\a idx] */
function var opt float: element(var int: idx, array[int] of var opt float: x) =
  let {
    var opt float: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

/** Return \a x[\a idx1, \a idx2] */
function var opt float: element(var int: idx1, var int: idx2, array[int,int] of var opt float: x) =
  let {
    var opt float: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

/** Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt float: element(var opt int: idx, array[int] of var opt float: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt float: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt float: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

%%%%%%%%%%%%%%%%%
% Element on sets

function var set of int: element_t(var int: idx, array[int] of var set of int: x) :: promise_total =
  let {
    var set of min(ub_array(x))..max(ub_array(x)): r ::is_defined_var;
    constraint idx in index_set(x);
    constraint array_var_set_element_nonshifted(idx,x,r) ::defines_var(r);
  } in r;

function var set of int: element_mt(var int: idx, array[int] of var set of int: x) :: promise_total =
  let {
    var set of min(ub_array(x))..max(ub_array(x)): r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_set_element_nonshifted(idx2,x,r) ::defines_var(r);
  } in r;

function var set of int: element_t(var int: idx1, var int: idx2, array[int,int] of var set of int: x) :: promise_total =
  let {
    var set of min(ub_array(x))..max(ub_array(x)): r ::is_defined_var;
    constraint idx1 in index_set_1of2(x);
    constraint idx2 in index_set_2of2(x);
    constraint array_var_set_element2d_nonshifted(idx1,idx2,x,r) ::defines_var(r);
  } in r;

function var set of int: element_mt(var int: idx1, var int: idx2, array[int,int] of var set of int: x) :: promise_total =
  let {
    var set of min(ub_array(x))..max(ub_array(x)): r ::is_defined_var;
    var min(index_set_1of2(x))..max(index_set_1of2(x)): idx1_2;
    var min(index_set_2of2(x))..max(index_set_2of2(x)): idx2_2;
    constraint (idx1 in index_set_1of2(x) /\ idx2 in index_set_2of2(x)) -> (idx1_2=idx1 /\ idx2_2=idx2);
    constraint (idx1 in index_set_1of2(x) /\ idx2 in index_set_2of2(x)) \/ (idx1_2=min(index_set_1of2(x)) /\ idx2_2=min(index_set_2of2(x)));
    constraint array_var_set_element2d_nonshifted(idx1_2,idx2_2,x,r) ::defines_var(r);
  } in r;

function var set of int: element(var int: idx, array[int] of var set of int: x) =
  if mzn_in_root_context(idx) then let {
    constraint idx in index_set(x)
  } in element_t(idx,x)
  elseif (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var set of int: element(var int: idx1, var int: idx2,
                                 array[int,int] of var set of int: x) =
  let {
    int: dim = card(index_set_2of2(x));
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_t(idx1,idx2,x)
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x))) )
       then
         element_t(idx1,idx2,x)
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt(idx1,idx2,x)
       endif;

function var set of int: element(var int: idx1, var int: idx2, var int: idx3,
                                 array[int,int,int] of var set of int: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x))) )
       then
         element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       endif;

function var set of int: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4,
                                 array[int,int,int,int] of var set of int: x) =
  let {
    int: dim2 = card(index_set_2of4(x));
    int: dim3 = card(index_set_3of4(x));
    int: dim4 = card(index_set_4of4(x));
    
    int: min = min(index_set_1of4(x))*dim2*dim3*dim4+
               min(index_set_2of4(x))*dim3*dim4+
               min(index_set_3of4(x))*dim4+
               min(index_set_4of4(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       endif;

function var set of int: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4, var int: idx5,
                                 array[int,int,int,int,int] of var set of int: x) =
  let {
    int: dim2 = card(index_set_2of5(x));
    int: dim3 = card(index_set_3of5(x));
    int: dim4 = card(index_set_4of5(x));
    int: dim5 = card(index_set_5of5(x));
    
    int: min = min(index_set_1of5(x))*dim2*dim3*dim4*dim5+
               min(index_set_2of5(x))*dim3*dim4*dim5+
               min(index_set_3of5(x))*dim4*dim5+
               min(index_set_4of5(x))*dim5+
               min(index_set_5of5(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of5(x);
           constraint idx2 in index_set_2of5(x);
           constraint idx3 in index_set_3of5(x);
           constraint idx4 in index_set_4of5(x);
           constraint idx5 in index_set_5of5(x);
         } in element_t( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of5(x)) /\ ub(idx1) <= max(index_set_1of5(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of5(x)) /\ ub(idx2) <= max(index_set_2of5(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of5(x)) /\ ub(idx3) <= max(index_set_3of5(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of5(x)) /\ ub(idx4) <= max(index_set_4of5(x)))
       /\ (has_bounds(idx5) /\ lb(idx5) >= min(index_set_5of5(x)) /\ ub(idx5) <= max(index_set_5of5(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of5(x);
         constraint idx2 in index_set_2of5(x);
         constraint idx3 in index_set_3of5(x);
         constraint idx4 in index_set_4of5(x);
         constraint idx5 in index_set_5of5(x);
       } in element_mt( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       endif;

function var set of int: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4, var int: idx5, var int: idx6,
                                 array[int,int,int,int,int,int] of var set of int: x) =
  let {
    int: dim2 = card(index_set_2of6(x));
    int: dim3 = card(index_set_3of6(x));
    int: dim4 = card(index_set_4of6(x));
    int: dim5 = card(index_set_5of6(x));
    int: dim6 = card(index_set_6of6(x));
    
    int: min = min(index_set_1of6(x))*dim2*dim3*dim4*dim5*dim6+
               min(index_set_2of6(x))*dim3*dim4*dim5*dim6+
               min(index_set_3of6(x))*dim4*dim5*dim6+
               min(index_set_4of6(x))*dim5*dim6+
               min(index_set_5of6(x))*dim6+
               min(index_set_6of6(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of6(x);
           constraint idx2 in index_set_2of6(x);
           constraint idx3 in index_set_3of6(x);
           constraint idx4 in index_set_4of6(x);
           constraint idx5 in index_set_5of6(x);
           constraint idx6 in index_set_6of6(x);
         } in element_t( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of6(x)) /\ ub(idx1) <= max(index_set_1of6(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of6(x)) /\ ub(idx2) <= max(index_set_2of6(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of6(x)) /\ ub(idx3) <= max(index_set_3of6(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of6(x)) /\ ub(idx4) <= max(index_set_4of6(x)))
       /\ (has_bounds(idx5) /\ lb(idx5) >= min(index_set_5of6(x)) /\ ub(idx5) <= max(index_set_5of6(x)))
       /\ (has_bounds(idx6) /\ lb(idx6) >= min(index_set_6of6(x)) /\ ub(idx6) <= max(index_set_6of6(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of6(x);
         constraint idx2 in index_set_2of6(x);
         constraint idx3 in index_set_3of6(x);
         constraint idx4 in index_set_4of6(x);
         constraint idx5 in index_set_5of6(x);
         constraint idx6 in index_set_6of6(x);
       } in element_mt( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       endif;

%%%%%%%%%%%%%%%%%%
% Element on bools

function var bool: element_t(var int: idx, array[int] of var bool: x) :: promise_total =
  let {
    var bool: r ::is_defined_var;
    constraint idx in index_set(x);
    constraint array_var_bool_element_nonshifted(idx,x,r) ::defines_var(r);
  } in r;

function var bool: element_mt(var int: idx, array[int] of var bool: x) :: promise_total =
  let {
    var bool: r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_bool_element_nonshifted(idx2,x,r) ::defines_var(r);
  } in r;

function var bool: element_t(var int: idx1, var int: idx2, array[int,int] of var bool: x) :: promise_total =
  let {
    var bool: r ::is_defined_var;
    constraint idx1 in index_set_1of2(x);
    constraint idx2 in index_set_2of2(x);
    constraint array_var_bool_element2d_nonshifted(idx1,idx2,x,r) ::defines_var(r);
  } in r;

function var bool: element_mt(var int: idx1, var int: idx2, array[int,int] of var bool: x) :: promise_total =
  let {
    var bool: r ::is_defined_var;
    var min(index_set_1of2(x))..max(index_set_1of2(x)): idx1_2;
    var min(index_set_2of2(x))..max(index_set_2of2(x)): idx2_2;
    constraint (idx1 in index_set_1of2(x) /\ idx2 in index_set_2of2(x)) -> (idx1_2=idx1 /\ idx2_2=idx2);
    constraint (idx1 in index_set_1of2(x) /\ idx2 in index_set_2of2(x)) \/ (idx1_2=min(index_set_1of2(x)) /\ idx2_2=min(index_set_2of2(x)));
    constraint array_var_bool_element2d_nonshifted(idx1_2,idx2_2,x,r) ::defines_var(r);
  } in r;

function var bool: element(var int: idx, array[int] of var bool: x) =
  if mzn_in_root_context(idx) then
    idx in index_set(x) /\ element_t(idx,x)
  elseif (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else idx in index_set(x) /\ element_mt(idx,x)
  endif;

function var bool: element(var int: idx1, var int: idx2,
                           array[int,int] of var bool: x) =
  let {
    int: dim = card(index_set_2of2(x));
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_t(idx1, idx2, x)
       elseif
       ((has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
        /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x))))
       then
         element_t(idx1,idx2,x)
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt(idx1,idx2,x)
       endif;

function var bool: element(var int: idx1, var int: idx2, var int: idx3,
                            array[int,int,int] of var bool: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x))) )
       then
         element_t( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt( (idx1*(dim2*dim3)+idx2*dim3+idx3-min)::domain, array1d(x))
       endif;

function var bool: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4,
                            array[int,int,int,int] of var bool: x) =
  let {
    int: dim2 = card(index_set_2of4(x));
    int: dim3 = card(index_set_3of4(x));
    int: dim4 = card(index_set_4of4(x));
    
    int: min = min(index_set_1of4(x))*dim2*dim3*dim4+
               min(index_set_2of4(x))*dim3*dim4+
               min(index_set_3of4(x))*dim4+
               min(index_set_4of4(x))-1;
  } in if mzn_in_root_context(idx1) then
       let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt( (idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min)::domain, array1d(x))
       endif;

function var bool: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4, var int: idx5,
                           array[int,int,int,int,int] of var bool: x) =
  let {
    int: dim2 = card(index_set_2of5(x));
    int: dim3 = card(index_set_3of5(x));
    int: dim4 = card(index_set_4of5(x));
    int: dim5 = card(index_set_5of5(x));
    
    int: min = min(index_set_1of5(x))*dim2*dim3*dim4*dim5+
               min(index_set_2of5(x))*dim3*dim4*dim5+
               min(index_set_3of5(x))*dim4*dim5+
               min(index_set_4of5(x))*dim5+
               min(index_set_5of5(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of5(x);
           constraint idx2 in index_set_2of5(x);
           constraint idx3 in index_set_3of5(x);
           constraint idx4 in index_set_4of5(x);
           constraint idx5 in index_set_5of5(x);
         } in element_t( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of5(x)) /\ ub(idx1) <= max(index_set_1of5(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of5(x)) /\ ub(idx2) <= max(index_set_2of5(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of5(x)) /\ ub(idx3) <= max(index_set_3of5(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of5(x)) /\ ub(idx4) <= max(index_set_4of5(x)))
       /\ (has_bounds(idx5) /\ lb(idx5) >= min(index_set_5of5(x)) /\ ub(idx5) <= max(index_set_5of5(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of5(x);
         constraint idx2 in index_set_2of5(x);
         constraint idx3 in index_set_3of5(x);
         constraint idx4 in index_set_4of5(x);
         constraint idx5 in index_set_5of5(x);
       } in element_mt( (idx1*(dim2*dim3*dim4*dim5)+idx2*(dim3*dim4*dim5)+idx3*(dim4*dim5)+idx4*dim5+idx5-min)::domain, array1d(x))
       endif;

function var bool: element(var int: idx1, var int: idx2, var int: idx3, var int: idx4, var int: idx5, var int: idx6,
                           array[int,int,int,int,int,int] of var bool: x) =
  let {
    int: dim2 = card(index_set_2of6(x));
    int: dim3 = card(index_set_3of6(x));
    int: dim4 = card(index_set_4of6(x));
    int: dim5 = card(index_set_5of6(x));
    int: dim6 = card(index_set_6of6(x));
    
    int: min = min(index_set_1of6(x))*dim2*dim3*dim4*dim5*dim6+
               min(index_set_2of6(x))*dim3*dim4*dim5*dim6+
               min(index_set_3of6(x))*dim4*dim5*dim6+
               min(index_set_4of6(x))*dim5*dim6+
               min(index_set_5of6(x))*dim6+
               min(index_set_6of6(x))-1;
  } in if mzn_in_root_context(idx1) then
         let {
           constraint idx1 in index_set_1of6(x);
           constraint idx2 in index_set_2of6(x);
           constraint idx3 in index_set_3of6(x);
           constraint idx4 in index_set_4of6(x);
           constraint idx5 in index_set_5of6(x);
           constraint idx6 in index_set_6of6(x);
         } in element_t( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       elseif (
          (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of6(x)) /\ ub(idx1) <= max(index_set_1of6(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of6(x)) /\ ub(idx2) <= max(index_set_2of6(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of6(x)) /\ ub(idx3) <= max(index_set_3of6(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of6(x)) /\ ub(idx4) <= max(index_set_4of6(x)))
       /\ (has_bounds(idx5) /\ lb(idx5) >= min(index_set_5of6(x)) /\ ub(idx5) <= max(index_set_5of6(x)))
       /\ (has_bounds(idx6) /\ lb(idx6) >= min(index_set_6of6(x)) /\ ub(idx6) <= max(index_set_6of6(x))) )
       then
         element_t( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       else let {
         constraint idx1 in index_set_1of6(x);
         constraint idx2 in index_set_2of6(x);
         constraint idx3 in index_set_3of6(x);
         constraint idx4 in index_set_4of6(x);
         constraint idx5 in index_set_5of6(x);
         constraint idx6 in index_set_6of6(x);
       } in element_mt( (idx1*(dim2*dim3*dim4*dim5*dim6)+idx2*(dim3*dim4*dim5*dim6)+idx3*(dim4*dim5*dim6)+idx4*(dim5*dim6)+idx5*dim6+idx6-min)::domain, array1d(x))
       endif;

/** Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt bool: element(var opt int: idx, array[int] of var bool: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt bool: element(var opt int: idx1, var opt int: idx2, array[int,int] of var bool: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** Return \a x[\a idx] */
function var opt bool: element(var int: idx, array[int] of var opt bool: x) =
  let {
    var opt bool: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

/** Return \a x[\a idx1, \a idx2] */
function var opt bool: element(var int: idx1, var int: idx2, array[int,int] of var opt bool: x) =
  let {
    var opt bool: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

/** Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt bool: element(var opt int: idx, array[int] of var opt bool: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt bool: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt bool: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

%-----------------------------------------------------------------------------%
%
% Internal functions for implementing div, mod etc

function set of int:compute_div_bounds(var int: x, var int: y);

function var int: div_t(var int: x, var int: y) :: promise_total =
  let {
    var (compute_div_bounds(x,y)): z ::is_defined_var;
    constraint y != 0;
    constraint int_div(x,y,z) ::defines_var(z); } in z;

function var int: div_mt(var int: x, var int: y) :: promise_total =
  let {
    var ((dom(y) diff {0}) union {1}): yy = if y=0 then 1 else y endif;
  } in div_t(x,yy);

function var float: fldiv_t(var float: x, float: y) :: promise_total = x*(1.0/y);

function var float: fldiv_t(var float: x, var float: y) :: promise_total =
  let {
    var float: z ::is_defined_var; % TODO: Compute division boundaries
    constraint if lb(y) <= 0 /\ ub(y) >= 0 then y != 0.0 endif;
    constraint float_div(x, y, z) ::defines_var(z); } in z;

function var float: fldiv_mt(var float: x, var float: y) :: promise_total =
  let {
    var (lb(y)..ub(y) diff {0.0}) union {1.0}: yy = if (y = 0.0) then 1.0 else y endif;
  } in fldiv_t(x, yy);

function var int: mod_t(var int: x, var int: y) :: promise_total =
  let { var -(max(ub(y),-lb(y)))..max(ub(y),-lb(y)): z;
        constraint y != 0;
        constraint int_mod(x,y,z); } in z;

function var int: mod_mt(var int: x, var int: y) :: promise_total =
  let {
    var {1} union dom(y): yy = if y=0 then 1 else y endif;
  } in mod_t(x,yy);

function var int:   product_rec(array[int] of var int: x) =
  if length(x)=0 then 1
  elseif length(x)=1 then x[min(index_set(x))]
  else let {
    array[int] of var int: xx = array1d(x);
    array[index_set(xx)] of var int: y;
    constraint y[1] = xx[1];
    constraint forall (i in 2..length(y)) (y[i]=y[i-1]*xx[i]);
  } in y[length(y)]
  endif;  

function var float:   product_rec(array[int] of var float: x) =
  if length(x)=0 then 1.0
  elseif length(x)=1 then x[min(index_set(x))]
  else let {
    array[int] of var float: xx = array1d(x);
    array[index_set(xx)] of var float: y;
    constraint y[1] = xx[1];
    constraint forall (i in 2..length(y)) (y[i]=y[i-1]*xx[i]);
  } in y[length(y)]
  endif;  

function var int: max_t(array[int] of var int: x) :: promise_total =
  if length(x)=0 then 0
  elseif length(x)=1 then x[min(index_set(x))]
  elseif length(x)=2 then max(x[1],x[2])
  else let {
    var lb_array(x)..ub_array(x): m ::is_defined_var;
    constraint array_int_maximum(m,x) ::defines_var(m);
  } in m
  endif;

function var int: min_t(array[int] of var int: x) :: promise_total =
  if length(x)=0 then 0
  elseif length(x)=1 then x[1]
  elseif length(x)=2 then min(x[1],x[2])
  else let {
    var lb_array(x)..ub_array(x): m ::is_defined_var;
    constraint array_int_minimum(m,x) ::defines_var(m);
  } in m
  endif;

function var float: max_t(array[int] of var float: x) :: promise_total =
  if length(x)=0 then 0.0
  elseif length(x)=1 then x[min(index_set(x))]
  elseif length(x)=2 then max(x[1],x[2])
  else let {
    var lb_array(x)..ub_array(x): m ::is_defined_var;
    constraint array_float_maximum(m,x) ::defines_var(m);
  } in m
  endif;

function var float: min_t(array[int] of var float: x) :: promise_total =
  if length(x)=0 then 0.0
  elseif length(x)=1 then x[1]
  elseif length(x)=2 then min(x[1],x[2])
  else let {
    var lb_array(x)..ub_array(x): m ::is_defined_var;
    constraint array_float_minimum(m,x) ::defines_var(m);
  } in m
  endif;

/* These predicates are used to intercept symmetry_breaking_constraint and
 * redundant_constraint calls in user models, so that they can be ignored
 * if the corresponding options have been set.
 */

predicate mzn_symmetry_breaking_constraint(var bool: b);
predicate mzn_redundant_constraint(var bool: b);

/* Annotations used for bookkeeping by the compiler */

annotation mzn_was_undefined;

/* Predicate used in the compiler for translating infinite domains into constraints */

predicate mzn_set_in_internal(var int: x, set of int: s) =
  if min(s) != -infinity /\ max(s) != infinity then set_in(x,s)
  else let {
    array[int] of int: ranges_ = set_to_ranges(s);
    array[int,1..2] of int: ranges = array2d(1..length(ranges_) div 2,1..2,ranges_);
    int: max_range = card(index_set_1of2(ranges));
  } in if max_range=2 /\ ranges[1,2]=ranges[2,1]-2 then
        x != ranges[1,2]+1
      else
        x <= ranges[1,2] \/ x >= ranges[max_range,1] \/
        exists (i in 2..max_range-1) (x in ranges[i,1]..ranges[i,2])
      endif
  endif;
