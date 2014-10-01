include "globals.mzn";
% domain constraints
predicate var_dom(var int:x, set of int: s) =
  if has_bounds(x) /\ dom(x) subset s then true
  else x in s
  endif;
predicate var_dom(var set of int: x, set of int: s) =
  if has_ub_set(x) /\ ub(x) subset s then true
  else set_subset(x,s)
  endif;

predicate var_dom(array[$T] of var set of int: x, set of int: d) =
    let { array[int] of var set of int: xx = array1d(1..length(x),x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));
predicate var_dom(array[$T] of var int: x, set of int: d) =
    let { array[int] of var int: xx = array1d(1..length(x),x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));

test var_dom(array[$T] of set of int: x, set of int: d) =
    let { array[int] of set of int: xx = array1d(1..length(x),x) }
    in forall (i in index_set(xx)) (xx[i] subset d);
test var_dom(array[$T] of int: x, set of int: d) =
    let { array[int] of int: xx = array1d(1..length(x),x) }
    in forall (i in index_set(xx)) (xx[i] in d);

function var set of int: array2set(array[int] of var int: x) ::promise_total =
    let {
      var set of int: y = array_union([
        let { var set of dom(x[i]): s;
        constraint x[i] in s /\ card(s)=1;
        } in s | i in index_set(x)]);
    } in y;

function set of int: array2set(array[int] of int: x) =
  { x[i] | i in index_set(x) };

% integer constraints
predicate int_abs(var int: a, var int: b);
predicate int_eq(var int: a, var int: b);
predicate int_eq_reif(var int: a, var int: b, var bool: r);
predicate int_le(var int: a, var int: b);
predicate int_le_reif(var int: a, var int: b, var bool: r);
predicate int_lin_eq(array[int] of int: as, array[int] of var int: bs, int: c);
predicate int_lin_eq_reif(array[int] of int: as, array[int] of var int: bs,int: c, var bool: r);
predicate int_lin_ne(array[int] of int: as, array[int] of var int: bs, int: c);
predicate int_lin_ne_reif(array[int] of int: as, array[int] of var int: bs,int: c, var bool: r);

predicate int_lin_le(array[int] of int: as, array[int] of var int: bs, int: c);
predicate int_lin_le_reif(array[int] of int: as, array[int] of var int: bs,int: c, var bool: r);
predicate int_ne(var int: a, var int: b);
predicate int_ne_reif(var int: a, var int: b, var bool: r);
predicate int_plus(var int: a, var int: b, var int: c);
predicate int_times_le(var int: a, var int: b, var int: c);
predicate int_div(var int: a, var int: b, var int: c);
predicate int_lt(var int: a, var int: b);
predicate int_lt_reif(var int: a, var int: b, var bool: r);
predicate int_max(var int: a, var int: b, var int: c);
predicate int_min(var int: a, var int: b, var int: c);
predicate int_mod(var int: a, var int: b, var int: c);
predicate int_times(var int: a, var int: b, var int: c);

%set constraints
predicate set_in(var int, set of int);
predicate set_in(var int, var set of int);
predicate set_in_reif(var int, set of int, var bool);
predicate set_in_reif(var int, var set of int, var bool);
predicate set_subset(var set of int, var set of int);
predicate set_subset_reif(var set of int, var set of int, var bool);
predicate set_le(var set of int, var set of int);
predicate set_lt(var set of int, var set of int);
predicate set_eq(var set of int, var set of int);
predicate set_eq_reif(var set of int, var set of int, var bool);
predicate set_ne(var set of int, var set of int);
predicate set_ne_reif(var set of int, var set of int, var bool);
function var set of int: set_intersect(var set of int, var set of int);
function var set of int: set_union(var set of int, var set of int);
function var set of int: set_diff(var set of int, var set of int);
function var set of int: set_symdiff(var set of int, var set of int);

% Float constraints
predicate float_abs(var float, var float);
predicate float_acos(var float, var float);
predicate float_acosh(var float, var float);
predicate float_asin(var float, var float);
predicate float_asinh(var float, var float);
predicate float_atan(var float, var float);
predicate float_atanh(var float, var float);
predicate float_cos(var float, var float);
predicate float_cosh(var float, var float);
predicate float_exp(var float, var float);
predicate float_ln(var float, var float);
predicate float_log10(var float, var float);
predicate float_log2(var float, var float);
predicate float_sqrt(var float, var float);
predicate float_sin(var float, var float);
predicate float_sinh(var float, var float);
predicate float_tan(var float, var float);
predicate float_tanh(var float, var float);

predicate float_eq(var float, var float);
predicate float_eq_reif(var float, var float, var bool);
predicate float_le(var float, var float);
predicate float_le_reif(var float, var float, var bool);
predicate float_lt(var float, var float);
predicate float_lt_reif(var float, var float, var bool);
predicate float_ne(var float, var float);
predicate float_ne_reif(var float, var float, var bool);

predicate float_lin_eq(array[int] of float, array[int] of var float, float);
predicate float_lin_eq_reif(array[int] of float, array[int] of var float, float, var bool);
predicate float_lin_le(array[int] of float, array[int] of var float, float);
predicate float_lin_le_reif(array[int] of float, array[int] of var float, float, var bool);
predicate float_lin_lt(array[int] of float, array[int] of var float, float);
predicate float_lin_lt_reif(array[int] of float, array[int] of var float, float, var bool);
predicate float_lin_ne(array[int] of float, array[int] of var float, float);
predicate float_lin_ne_reif(array[int] of float, array[int] of var float, float, var bool);

predicate float_max(var float, var float, var float);
predicate float_min(var float, var float, var float);
predicate float_plus(var float, var float, var float);

% Array constraints
predicate array_bool_and(array[int] of var bool: as, var bool: r);
predicate array_bool_or(array[int] of var bool: as, var bool: r);
predicate array_bool_xor(array[int] of var bool: as);
predicate array_bool_element(var int: b, array[int] of bool: as, var bool: c);
predicate array_int_element(var int: b, array[int] of int: as, var int: c);
predicate array_float_element(var int: b, array[int] of float: as, var float: c);
predicate array_set_element(var int: b, array[int] of set of int: as, var set of int: c);
predicate array_var_bool_element(var int: b, array[int] of var bool: as, var bool: c);
predicate array_var_int_element(var int: b, array[int] of var int: as, var int: c);
predicate array_var_float_element(var int: b, array[int] of var float: as, var float: c);
predicate array_var_set_element(var int: b, array[int] of var set of int: as, var set of int: c);

predicate array_var_int_element(var int: x, array[int] of int: y, var int: z) =
  array_int_element(x,y,z);
predicate array_var_bool_element(var int: x, array[int] of bool: y, var bool: z) =
  array_bool_element(x,y,z);
predicate array_var_float_element(var int: x, array[int] of float: y, var float: z) =
  array_float_element(x,y,z);
predicate array_var_set_element(var int: x, array[int] of set of int: y, var set of int: z) =
  array_set_element(x,y,z);

%Boolean constraints
predicate bool2int(var bool: a, var int: b);
predicate bool_and(var bool: a, var bool: b, var bool: r);
predicate bool_clause(array[int] of var bool: as, array[int] of var bool: bs);
predicate bool_eq(var bool: a, var bool: b);
predicate bool_eq_reif(var bool: a, var bool: b, var bool: r);
predicate bool_le(var bool: a, var bool: b);
predicate bool_le_reif(var bool: a, var bool: b, var bool: r);
predicate bool_lin_eq(array[int] of int: as, array[int] of var bool: bs, var int: c);
predicate bool_lin_le(array[int] of int: as, array[int] of var bool: bs, int: c);
predicate bool_lt(var bool: a, var bool: b);
predicate bool_lt_reif(var bool: a, var bool: b, var bool: r);
predicate bool_not(var bool: a, var bool: b);
predicate bool_or(var bool: a, var bool: b, var bool: r);
predicate bool_xor(var bool: a, var bool: b, var bool: r);
predicate bool_xor(var bool: a, var bool: b);

predicate bool_xor_reif(var bool: a, var bool: b, var bool: c) =
  bool_xor(a,b,c);

predicate xorall(array[int] of var bool: b) = array_bool_xor(b);

predicate xorall_reif(array[int] of var bool: b, var bool: c) =
  xorall([not c]++b);

function var int: lin_exp(array[int] of int, array[int] of var int, int);
function var float: lin_exp(array[int] of float, array[int] of var float, float);

% Element constraints

function var int: element_t(var int: idx, array[int] of var int: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    constraint array_var_int_element((idx-(min(index_set(x))-1))::domain,[x[i] | i in index_set(x)],r) ::defines_var(r);
  } in r;

function var int: element_mt(var int: idx, array[int] of var int: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_int_element((idx2-(min(index_set(x))-1))::domain,[x[i] | i in index_set(x)],r) ::defines_var(r);
  } in r;

function var int: element(var int: idx, array[int] of var int: x) =
  if (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var int: element(var int: idx1, int: idx2,
                          array[int,int] of var int: x) =
  element(idx1,
          array1d(min(index_set_1of2(x))..max(index_set_1of2(x)),
                  [x[i,idx2] | i in index_set_1of2(x)]));

function var int: element(int: idx1, var int: idx2,
                          array[int,int] of var int: x) =
  element(idx2,
          array1d(min(index_set_2of2(x))..max(index_set_2of2(x)),
                  [x[idx1,i] | i in index_set_2of2(x)]));

function var int: element(var int: idx1, var int: idx2,
                          array[int,int] of var int: x) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x)))
       then
         element_t(idx1*dim+idx2-min_flat, array1d(1..dim*card(index_set_1of2(x)),x))
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt(idx1*dim+idx2-min_flat, array1d(1..dim*card(index_set_1of2(x)),x))
       endif;

function var int: element(var int: idx1, var int: idx2, var int: idx3,
                          array[int,int,int] of var int: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x)))
       then
         element_t(idx1*(dim2*dim3)+idx2*dim3+idx3-min, array1d(1..dim2*dim3*card(index_set_1of3(x)),x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt(idx1*(dim2*dim3)+idx2*dim3+idx3-min, array1d(1..dim2*dim3*card(index_set_1of3(x)),x))
       endif;

function var int: element(var int: idx1, int: idx2, int: idx3,
                          array[int,int,int] of var int: x) =
  element(idx1, array1d(min(index_set_1of3(x))..max(index_set_1of3(x)), [ x[i,idx2,idx3] | i in index_set_1of3(x)]));

function var int: element(int: idx1, var int: idx2, int: idx3,
                          array[int,int,int] of var int: x) =
  element(idx2, array1d(min(index_set_2of3(x))..max(index_set_2of3(x)), [ x[idx1,i,idx3] | i in index_set_2of3(x)]));

function var int: element(int: idx1, int: idx2, var int: idx3,
                          array[int,int,int] of var int: x) =
  element(idx3, array1d(min(index_set_3of3(x))..max(index_set_3of3(x)), [ x[idx1,idx2,i] | i in index_set_3of3(x)]));

function var int: element(var int: idx1, var int: idx2, int: idx3,
                          array[int,int,int] of var int: x) =
  element(idx1, idx2, array2d(min(index_set_1of3(x))..max(index_set_1of3(x)),
                              min(index_set_2of3(x))..max(index_set_2of3(x)),
                              [ x[i,j,idx3] | i in index_set_1of3(x), j in index_set_2of3(x)]));

function var int: element(var int: idx1, int: idx2, var int: idx3,
                          array[int,int,int] of var int: x) =
  element(idx1, idx3, array2d(min(index_set_1of3(x))..max(index_set_1of3(x)),
                              min(index_set_3of3(x))..max(index_set_3of3(x)),
                              [ x[i,idx2,j] | i in index_set_1of3(x), j in index_set_3of3(x)]));

function var int: element(int: idx1, var int: idx2, var int: idx3,
                          array[int,int,int] of var int: x) =
  element(idx2, idx3, array2d(min(index_set_2of3(x))..max(index_set_2of3(x)),
                              min(index_set_3of3(x))..max(index_set_3of3(x)),
                              [ x[idx1,i,j] | i in index_set_2of3(x), j in index_set_3of3(x)]));

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
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x)))
       then
         element_t(idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min, array1d(1..dim2*dim3*dim4*card(index_set_1of4(x)),x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt(idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min, array1d(1..dim2*dim3*dim4*card(index_set_1of4(x)),x))
       endif;

function var int: element(var int: idx1, var int: idx2, var int: idx3, int: idx4,
                          array[int,int,int,int] of var int: x) =
  element(idx1, idx2, idx3, array3d(index_set_1of4(x),index_set_2of4(x),index_set_3of4(x), [ x[i,j,k,idx4] | i in index_set_1of4(x), j in index_set_2of4(x), k in index_set_3of4(x)]));

function var int: element(var int: idx1, var int: idx2, int: idx3, var int: idx4,
                          array[int,int,int,int] of var int: x) =
  element(idx1, idx2, idx4, array3d(index_set_1of4(x),index_set_2of4(x),index_set_4of4(x), [ x[i,j,idx3,k] | i in index_set_1of4(x), j in index_set_2of4(x), k in index_set_4of4(x)]));

function var int: element(var int: idx1, int: idx2, var int: idx3, var int: idx4,
                          array[int,int,int,int] of var int: x) =
  element(idx1, idx3, idx4, array3d(index_set_1of4(x),index_set_3of4(x),index_set_4of4(x), [ x[i,idx2,j,k] | i in index_set_1of4(x), j in index_set_3of4(x), k in index_set_4of4(x)]));

function var int: element(int: idx1, var int: idx2, var int: idx3, var int: idx4,
                          array[int,int,int,int] of var int: x) =
  element(idx2, idx3, idx4, array3d(index_set_2of4(x),index_set_3of4(x),index_set_4of4(x), [ x[idx1,i,j,k] | i in index_set_2of4(x), j in index_set_3of4(x), k in index_set_4of4(x)]));

%%%%%%%%%%%%%%%%%%%
% Element on floats

function var float: element_t(var int: idx, array[int] of var float: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    constraint array_var_float_element((idx-(min(index_set(x))-1))::domain,[x[i] | i in index_set(x)],r) ::defines_var(r);
  } in r;

function var float: element_mt(var int: idx, array[int] of var float: x) :: promise_total =
  let {
    var lb_array(x)..ub_array(x): r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_float_element((idx2-(min(index_set(x))-1))::domain,[x[i] | i in index_set(x)],r) ::defines_var(r);
  } in r;

function var float: element(var int: idx, array[int] of var float: x) =
  if (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var float: element(var int: idx1, int: idx2,
                            array[int,int] of var float: x) =
  element(idx1,
          array1d(min(index_set_1of2(x))..max(index_set_1of2(x)),
                  [x[i,idx2] | i in index_set_1of2(x)]));

function var float: element(int: idx1, var int: idx2,
                            array[int,int] of var float: x) =
  element(idx2,
          array1d(min(index_set_2of2(x))..max(index_set_2of2(x)),
                  [x[idx1,i] | i in index_set_2of2(x)]));

function var float: element(var int: idx1, var int: idx2,
                            array[int,int] of var float: x) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x)))
       then
         element_t(idx1*dim+idx2-min_flat, array1d(1..dim*card(index_set_1of2(x)),x))
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt(idx1*dim+idx2-min_flat, array1d(1..dim*card(index_set_1of2(x)),x))
       endif;

function var float: element(var int: idx1, var int: idx2, var int: idx3,
                            array[int,int,int] of var float: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x)))
       then
         element_t(idx1*(dim2*dim3)+idx2*dim3+idx3-min, array1d(1..dim2*dim3*card(index_set_1of3(x)),x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt(idx1*(dim2*dim3)+idx2*dim3+idx3-min, array1d(1..dim2*dim3*card(index_set_1of3(x)),x))
       endif;

function var float: element(var int: idx1, int: idx2, int: idx3,
                          array[int,int,int] of var float: x) =
  element(idx1, array1d(min(index_set_1of3(x))..max(index_set_1of3(x)), [ x[i,idx2,idx3] | i in index_set_1of3(x)]));

function var float: element(int: idx1, var int: idx2, int: idx3,
                          array[int,int,int] of var float: x) =
  element(idx2, array1d(min(index_set_2of3(x))..max(index_set_2of3(x)), [ x[idx1,i,idx3] | i in index_set_2of3(x)]));

function var float: element(int: idx1, int: idx2, var int: idx3,
                          array[int,int,int] of var float: x) =
  element(idx3, array1d(min(index_set_3of3(x))..max(index_set_3of3(x)), [ x[idx1,idx2,i] | i in index_set_3of3(x)]));

function var float: element(var int: idx1, var int: idx2, int: idx3,
                          array[int,int,int] of var float: x) =
  element(idx1, idx2, array2d(min(index_set_1of3(x))..max(index_set_1of3(x)),
                              min(index_set_2of3(x))..max(index_set_2of3(x)),
                              [ x[i,j,idx3] | i in index_set_1of3(x), j in index_set_2of3(x)]));

function var float: element(var int: idx1, int: idx2, var int: idx3,
                          array[int,int,int] of var float: x) =
  element(idx1, idx3, array2d(min(index_set_1of3(x))..max(index_set_1of3(x)),
                              min(index_set_3of3(x))..max(index_set_3of3(x)),
                              [ x[i,idx2,j] | i in index_set_1of3(x), j in index_set_3of3(x)]));

function var float: element(int: idx1, var int: idx2, var int: idx3,
                          array[int,int,int] of var float: x) =
  element(idx2, idx3, array2d(min(index_set_2of3(x))..max(index_set_2of3(x)),
                              min(index_set_3of3(x))..max(index_set_3of3(x)),
                              [ x[idx1,i,j] | i in index_set_2of3(x), j in index_set_3of3(x)]));


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
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x)))
       then
         element_t(idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min, array1d(1..dim2*dim3*dim4*card(index_set_1of4(x)),x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt(idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min, array1d(1..dim2*dim3*dim4*card(index_set_1of4(x)),x))
       endif;

function var float: element(var int: idx1, var int: idx2, var int: idx3, int: idx4,
                            array[int,int,int,int] of var float: x) =
  element(idx1, idx2, idx3, array3d(index_set_1of4(x),index_set_2of4(x),index_set_3of4(x), [ x[i,j,k,idx4] | i in index_set_1of4(x), j in index_set_2of4(x), k in index_set_3of4(x)]));

function var float: element(var int: idx1, var int: idx2, int: idx3, var int: idx4,
                            array[int,int,int,int] of var float: x) =
  element(idx1, idx2, idx4, array3d(index_set_1of4(x),index_set_2of4(x),index_set_4of4(x), [ x[i,j,idx3,k] | i in index_set_1of4(x), j in index_set_2of4(x), k in index_set_4of4(x)]));

function var float: element(var int: idx1, int: idx2, var int: idx3, var int: idx4,
                            array[int,int,int,int] of var float: x) =
  element(idx1, idx3, idx4, array3d(index_set_1of4(x),index_set_3of4(x),index_set_4of4(x), [ x[i,idx2,j,k] | i in index_set_1of4(x), j in index_set_3of4(x), k in index_set_4of4(x)]));

function var float: element(int: idx1, var int: idx2, var int: idx3, var int: idx4,
                            array[int,int,int,int] of var float: x) =
  element(idx2, idx3, idx4, array3d(index_set_2of4(x),index_set_3of4(x),index_set_4of4(x), [ x[idx1,i,j,k] | i in index_set_2of4(x), j in index_set_3of4(x), k in index_set_4of4(x)]));

                      
%%%%%%%%%%%%%%%%%
% Element on sets

function var set of int: element_t(var int: idx, array[int] of var set of int: x) :: promise_total =
  let {
    var set of min(ub_array(x))..max(ub_array(x)): r ::is_defined_var;
    constraint array_var_set_element(idx-(min(index_set(x))-1),[x[i] | i in index_set(x)],r) ::defines_var(r);
  } in r;

function var set of int: element_mt(var int: idx, array[int] of var set of int: x) :: promise_total =
  let {
    var set of min(ub_array(x))..max(ub_array(x)): r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_set_element(idx2-(min(index_set(x))-1),[x[i] | i in index_set(x)],r) ::defines_var(r);
  } in r;

function var set of int: element(var int: idx, array[int] of var set of int: x) =
  if (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else let {
    constraint idx in index_set(x)
  } in element_mt(idx,x)
  endif;

function var set of int: element(var int: idx1, int: idx2,
                                 array[int,int] of var set of int: x) =
  element(idx1,
          array1d(min(index_set_1of2(x))..max(index_set_1of2(x)),
                  [x[i,idx2] | i in index_set_1of2(x)]));

function var set of int: element(int: idx1, var int: idx2,
                                 array[int,int] of var set of int: x) =
  element(idx2,
          array1d(min(index_set_2of2(x))..max(index_set_2of2(x)),
                  [x[idx1,i] | i in index_set_2of2(x)]));

function var set of int: element(var int: idx1, var int: idx2,
                                 array[int,int] of var set of int: x) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x)))
       then
         element_t(idx1*dim+idx2-min_flat, array1d(1..dim*card(index_set_1of2(x)),x))
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt(idx1*dim+idx2-min_flat, array1d(1..dim*card(index_set_1of2(x)),x))
       endif;

function var set of int: element(var int: idx1, var int: idx2, var int: idx3,
                                 array[int,int,int] of var set of int: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x)))
       then
         element_t(idx1*(dim2*dim3)+idx2*dim3+idx3-min, array1d(1..dim2*dim3*card(index_set_1of3(x)),x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt(idx1*(dim2*dim3)+idx2*dim3+idx3-min, array1d(1..dim2*dim3*card(index_set_1of3(x)),x))
       endif;

function var set of int: element(var int: idx1, int: idx2, int: idx3,
                                 array[int,int,int] of var set of int: x) =
  element(idx1, array1d(min(index_set_1of3(x))..max(index_set_1of3(x)), [ x[i,idx2,idx3] | i in index_set_1of3(x)]));

function var set of int: element(int: idx1, var int: idx2, int: idx3,
                                 array[int,int,int] of var set of int: x) =
  element(idx2, array1d(min(index_set_2of3(x))..max(index_set_2of3(x)), [ x[idx1,i,idx3] | i in index_set_2of3(x)]));

function var set of int: element(int: idx1, int: idx2, var int: idx3,
                                 array[int,int,int] of var set of int: x) =
  element(idx3, array1d(min(index_set_3of3(x))..max(index_set_3of3(x)), [ x[idx1,idx2,i] | i in index_set_3of3(x)]));

function var set of int: element(var int: idx1, var int: idx2, int: idx3,
                                 array[int,int,int] of var set of int: x) =
  element(idx1, idx2, array2d(min(index_set_1of3(x))..max(index_set_1of3(x)),
                              min(index_set_2of3(x))..max(index_set_2of3(x)),
                              [ x[i,j,idx3] | i in index_set_1of3(x), j in index_set_2of3(x)]));

function var set of int: element(var int: idx1, int: idx2, var int: idx3,
                                 array[int,int,int] of var set of int: x) =
  element(idx1, idx3, array2d(min(index_set_1of3(x))..max(index_set_1of3(x)),
                              min(index_set_3of3(x))..max(index_set_3of3(x)),
                              [ x[i,idx2,j] | i in index_set_1of3(x), j in index_set_3of3(x)]));

function var set of int: element(int: idx1, var int: idx2, var int: idx3,
                                 array[int,int,int] of var set of int: x) =
  element(idx2, idx3, array2d(min(index_set_2of3(x))..max(index_set_2of3(x)),
                              min(index_set_3of3(x))..max(index_set_3of3(x)),
                              [ x[idx1,i,j] | i in index_set_2of3(x), j in index_set_3of3(x)]));

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
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x)))
       then
         element_t(idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min, array1d(1..dim2*dim3*dim4*card(index_set_1of4(x)),x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt(idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min, array1d(1..dim2*dim3*dim4*card(index_set_1of4(x)),x))
       endif;

function var set of int: element(var int: idx1, var int: idx2, var int: idx3, int: idx4,
                                 array[int,int,int,int] of var set of int: x) =
  element(idx1, idx2, idx3, array3d(index_set_1of4(x),index_set_2of4(x),index_set_3of4(x), [ x[i,j,k,idx4] | i in index_set_1of4(x), j in index_set_2of4(x), k in index_set_3of4(x)]));

function var set of int: element(var int: idx1, var int: idx2, int: idx3, var int: idx4,
                                 array[int,int,int,int] of var set of int: x) =
  element(idx1, idx2, idx4, array3d(index_set_1of4(x),index_set_2of4(x),index_set_4of4(x), [ x[i,j,idx3,k] | i in index_set_1of4(x), j in index_set_2of4(x), k in index_set_4of4(x)]));

function var set of int: element(var int: idx1, int: idx2, var int: idx3, var int: idx4,
                                 array[int,int,int,int] of var set of int: x) =
  element(idx1, idx3, idx4, array3d(index_set_1of4(x),index_set_3of4(x),index_set_4of4(x), [ x[i,idx2,j,k] | i in index_set_1of4(x), j in index_set_3of4(x), k in index_set_4of4(x)]));

function var set of int: element(int: idx1, var int: idx2, var int: idx3, var int: idx4,
                                 array[int,int,int,int] of var set of int: x) =
  element(idx2, idx3, idx4, array3d(index_set_2of4(x),index_set_3of4(x),index_set_4of4(x), [ x[idx1,i,j,k] | i in index_set_2of4(x), j in index_set_3of4(x), k in index_set_4of4(x)]));

%%%%%%%%%%%%%%%%%%
% Element on bools

function var bool: element_t(var int: idx, array[int] of var bool: x) :: promise_total =
  let {
    var bool: r ::is_defined_var;
    constraint array_var_bool_element(idx-(min(index_set(x))-1),[x[i] | i in index_set(x)],r) ::defines_var(r);
  } in r;

function var bool: element_mt(var int: idx, array[int] of var bool: x) :: promise_total =
  let {
    var bool: r ::is_defined_var;
    var min(index_set(x))..max(index_set(x)): idx2;
    constraint idx in index_set(x) -> idx2=idx;
    constraint idx in index_set(x) \/ idx2=min(index_set(x));
    constraint array_var_bool_element(idx2-(min(index_set(x))-1),[x[i] | i in index_set(x)],r) ::defines_var(r);
  } in r;

function var bool: element(var int: idx, array[int] of var bool: x) =
  if (has_bounds(idx) /\ lb(idx) >= min(index_set(x)) /\ ub(idx) <= max(index_set(x))) then
    element_t(idx,x)
  else idx in index_set(x) /\ element_mt(idx,x)
  endif;

function var bool: element(var int: idx1, int: idx2,
                          array[int,int] of var bool: x) =
  element(idx1,
          array1d(min(index_set_1of2(x))..max(index_set_1of2(x)),
                  [x[i,idx2] | i in index_set_1of2(x)]));

function var bool: element(int: idx1, var int: idx2,
                          array[int,int] of var bool: x) =
  element(idx2,
          array1d(min(index_set_2of2(x))..max(index_set_2of2(x)),
                  [x[idx1,i] | i in index_set_2of2(x)]));

function var bool: element(var int: idx1, var int: idx2,
                          array[int,int] of var bool: x) =
  let {
    int: dim = card(index_set_2of2(x));
    int: min_flat = min(index_set_1of2(x))*dim+min(index_set_2of2(x))-1;
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of2(x)) /\ ub(idx1) <= max(index_set_1of2(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of2(x)) /\ ub(idx2) <= max(index_set_2of2(x)))
       then
         element_t(idx1*dim+idx2-min_flat, array1d(1..dim*card(index_set_1of2(x)),x))
       else
         let {
           constraint idx1 in index_set_1of2(x);
           constraint idx2 in index_set_2of2(x);
         } in element_mt(idx1*dim+idx2-min_flat, array1d(1..dim*card(index_set_1of2(x)),x))
       endif;

function var bool: element(var int: idx1, var int: idx2, var int: idx3,
                            array[int,int,int] of var bool: x) =
  let {
    int: dim2 = card(index_set_2of3(x));
    int: dim3 = card(index_set_3of3(x));
    
    int: min = min(index_set_1of3(x))*dim2*dim3+
               min(index_set_2of3(x))*dim3+
               min(index_set_3of3(x))-1;
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of3(x)) /\ ub(idx1) <= max(index_set_1of3(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of3(x)) /\ ub(idx2) <= max(index_set_2of3(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of3(x)) /\ ub(idx3) <= max(index_set_3of3(x)))
       then
         element_t(idx1*(dim2*dim3)+idx2*dim3+idx3-min, array1d(1..dim2*dim3*card(index_set_1of3(x)),x))
       else let {
         constraint idx1 in index_set_1of3(x);
         constraint idx2 in index_set_2of3(x);
         constraint idx3 in index_set_3of3(x);
       } in element_mt(idx1*(dim2*dim3)+idx2*dim3+idx3-min, array1d(1..dim2*dim3*card(index_set_1of3(x)),x))
       endif;

function var bool: element(var int: idx1, int: idx2, int: idx3,
                          array[int,int,int] of var bool: x) =
  element(idx1, array1d(min(index_set_1of3(x))..max(index_set_1of3(x)), [ x[i,idx2,idx3] | i in index_set_1of3(x)]));

function var bool: element(int: idx1, var int: idx2, int: idx3,
                          array[int,int,int] of var bool: x) =
  element(idx2, array1d(min(index_set_2of3(x))..max(index_set_2of3(x)), [ x[idx1,i,idx3] | i in index_set_2of3(x)]));

function var bool: element(int: idx1, int: idx2, var int: idx3,
                          array[int,int,int] of var bool: x) =
  element(idx3, array1d(min(index_set_3of3(x))..max(index_set_3of3(x)), [ x[idx1,idx2,i] | i in index_set_3of3(x)]));

function var bool: element(var int: idx1, var int: idx2, int: idx3,
                          array[int,int,int] of var bool: x) =
  element(idx1, idx2, array2d(min(index_set_1of3(x))..max(index_set_1of3(x)),
                              min(index_set_2of3(x))..max(index_set_2of3(x)),
                              [ x[i,j,idx3] | i in index_set_1of3(x), j in index_set_2of3(x)]));

function var bool: element(var int: idx1, int: idx2, var int: idx3,
                          array[int,int,int] of var bool: x) =
  element(idx1, idx3, array2d(min(index_set_1of3(x))..max(index_set_1of3(x)),
                              min(index_set_3of3(x))..max(index_set_3of3(x)),
                              [ x[i,idx2,j] | i in index_set_1of3(x), j in index_set_3of3(x)]));

function var bool: element(int: idx1, var int: idx2, var int: idx3,
                          array[int,int,int] of var bool: x) =
  element(idx2, idx3, array2d(min(index_set_2of3(x))..max(index_set_2of3(x)),
                              min(index_set_3of3(x))..max(index_set_3of3(x)),
                              [ x[idx1,i,j] | i in index_set_2of3(x), j in index_set_3of3(x)]));

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
  } in if (has_bounds(idx1) /\ lb(idx1) >= min(index_set_1of4(x)) /\ ub(idx1) <= max(index_set_1of4(x)))
       /\ (has_bounds(idx2) /\ lb(idx2) >= min(index_set_2of4(x)) /\ ub(idx2) <= max(index_set_2of4(x)))
       /\ (has_bounds(idx3) /\ lb(idx3) >= min(index_set_3of4(x)) /\ ub(idx3) <= max(index_set_3of4(x)))
       /\ (has_bounds(idx4) /\ lb(idx4) >= min(index_set_4of4(x)) /\ ub(idx4) <= max(index_set_4of4(x)))
       then
         element_t(idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min, array1d(1..dim2*dim3*dim4*card(index_set_1of4(x)),x))
       else let {
         constraint idx1 in index_set_1of4(x);
         constraint idx2 in index_set_2of4(x);
         constraint idx3 in index_set_3of4(x);
         constraint idx4 in index_set_4of4(x);
       } in element_mt(idx1*(dim2*dim3*dim4)+idx2*(dim3*dim4)+idx3*dim4+idx4-min, array1d(1..dim2*dim3*dim4*card(index_set_1of4(x)),x))
       endif;

function var bool: element(var int: idx1, var int: idx2, var int: idx3, int: idx4,
                            array[int,int,int,int] of var bool: x) =
  element(idx1, idx2, idx3, array3d(index_set_1of4(x),index_set_2of4(x),index_set_3of4(x), [ x[i,j,k,idx4] | i in index_set_1of4(x), j in index_set_2of4(x), k in index_set_3of4(x)]));

function var bool: element(var int: idx1, var int: idx2, int: idx3, var int: idx4,
                            array[int,int,int,int] of var bool: x) =
  element(idx1, idx2, idx4, array3d(index_set_1of4(x),index_set_2of4(x),index_set_4of4(x), [ x[i,j,idx3,k] | i in index_set_1of4(x), j in index_set_2of4(x), k in index_set_4of4(x)]));

function var bool: element(var int: idx1, int: idx2, var int: idx3, var int: idx4,
                            array[int,int,int,int] of var bool: x) =
  element(idx1, idx3, idx4, array3d(index_set_1of4(x),index_set_3of4(x),index_set_4of4(x), [ x[i,idx2,j,k] | i in index_set_1of4(x), j in index_set_3of4(x), k in index_set_4of4(x)]));

function var bool: element(int: idx1, var int: idx2, var int: idx3, var int: idx4,
                            array[int,int,int,int] of var bool: x) =
  element(idx2, idx3, idx4, array3d(index_set_2of4(x),index_set_3of4(x),index_set_4of4(x), [ x[idx1,i,j,k] | i in index_set_2of4(x), j in index_set_3of4(x), k in index_set_4of4(x)]));

% Comparisons
function     bool: '<'(    $T,    $T);
function var bool: '<'(var $T,var $T);
function     bool: '>'(    $T,    $T);
function var bool: '>'(var $T,var $T);
function     bool: '<='(    $T,    $T);
function var bool: '<='(var $T,var $T);
function     bool: '>='(    $T,    $T);
function var bool: '>='(var $T,var $T);
function     bool: '='(    $T,    $T);
function     bool: '='(opt $T, opt $T);
function var bool: '='(var $T,var $T);
function var bool: '='(var opt $T,var opt $T);
function     bool: '!='(    $T,    $T);
function var bool: '!='(var $T,var $T);

function     bool: '<'(array[$U] of     $T,array[$U] of     $T);
function var bool: '<'(array[$U] of var $T,array[$U] of var $T);
function     bool: '>'(array[$U] of     $T,array[$U] of     $T);
function var bool: '>'(array[$U] of var $T,array[$U] of var $T);
function     bool: '<='(array[$U] of     $T,array[$U] of     $T);
function var bool: '<='(array[$U] of var $T,array[$U] of var $T);
function     bool: '>='(array[$U] of     $T,array[$U] of     $T);
function var bool: '>='(array[$U] of var $T,array[$U] of var $T);

function     bool: '='(array[$T] of     int: x,array[$T] of     int: y) =
    let {
        array[int] of int: xx = array1d(1..length(x),x);
        array[int] of int: yy = array1d(1..length(y),y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

function var bool: '='(array[$T] of var int: x,array[$T] of var int: y) =
    let {
        array[int] of var int: xx = array1d(1..length(x),x);
        array[int] of var int: yy = array1d(1..length(y),y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

function     bool: '='(array[int] of     bool: x,array[int] of     bool: y) =
    let {
        array[int] of bool: xx = array1d(1..length(x),x);
        array[int] of bool: yy = array1d(1..length(y),y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

function var bool: '='(array[int] of var bool: x,array[int] of var bool: y) =
    let {
        array[int] of var bool: xx = array1d(1..length(x),x);
        array[int] of var bool: yy = array1d(1..length(y),y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

function     bool: '='(array[int] of     set of int: x,array[int] of     set of int: y) =
    let {
        array[int] of set of int: xx = array1d(1..length(x),x);
        array[int] of set of int: yy = array1d(1..length(y),y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

function var bool: '='(array[int] of var set of int: x,array[int] of var set of int: y) =
    let {
        array[int] of var set of int: xx = array1d(1..length(x),x);
        array[int] of var set of int: yy = array1d(1..length(y),y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

function     bool: '='(array[int] of     float: x,array[int] of     float: y) =
    let {
        array[int] of float: xx = array1d(1..length(x),x);
        array[int] of float: yy = array1d(1..length(y),y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

function var bool: '='(array[int] of var float: x,array[int] of var float: y) =
    let {
        array[int] of var float: xx = array1d(1..length(x),x);
        array[int] of var float: yy = array1d(1..length(y),y);
    } in
    assert(index_sets_agree(x,y), "array index sets do not match",
      forall (i in index_set(xx)) (xx[i]=yy[i])
    );

function     bool: '!='(array[$U] of     $T,array[$U] of     $T);
function var bool: '!='(array[$U] of var $T,array[$U] of var $T);

% Arithmetic operations
function     int:   '+'(    int,     int);
function var int:   '+'(var int,  var int);
function     float: '+'(    float,    float);
function var float: '+'(var float,var float);
function     int:   '-'(    int,     int);
function var int:   '-'(var int,  var int);
function     float: '-'(    float,    float);
function var float: '-'(var float,var float);
function     int:   '*'(    int,     int);
function var int:   '*'(var int,  var int);
function     float: '*'(    float,    float);
function var float: '*'(var float,var float);
function     int:   '-'(    int);
function var int:   '-'(var int);
function     float: '-'(    float);
function var float: '-'(var float);

function     int: 'div'(int,int);

function set of int:compute_div_bounds(var int: x, var int: y);

function var int: div_t(var int: x, var int: y) :: promise_total =
  let {
    var (compute_div_bounds(x,y)): z ::is_defined_var; 
    constraint int_div(x,y,z) ::defines_var(z); } in z;
function var int: div_mt(var int: x, var int: y) :: promise_total =
  let {
    var ((dom(y) diff {0}) union {1}): yy = if y=0 then 1 else y endif;
  } in div_t(x,yy);

function var int: 'div'(var int: x,var int: y) =
  if not (0 in dom(y)) then div_t(x,y) else
  let { constraint y != 0 } in div_mt(x,y) endif;
  
function     int: 'mod'(int,int);

function var int: mod_t(var int: x, var int: y) :: promise_total =
  let { var -(max(ub(y),-lb(y)))..max(ub(y),-lb(y)): z; constraint int_mod(x,y,z); } in z;
function var int: mod_mt(var int: x, var int: y) :: promise_total =
  let {
    var {1} union dom(y): yy = if y=0 then 1 else y endif;
  } in mod_t(x,yy);

function var int: 'mod'(var int: x,var int: y) =
  if not (0 in dom(y)) then mod_t(x,y) else
  let { constraint y != 0 } in mod_mt(x,y) endif;


function     float: '/'(    float,    float);
function var float: '/'(var float,var float);

function     int:   sum(array[$T] of     int);
function var int:   sum(array[$T] of var int: x);
function     float: sum(array[$T] of     float);
function var float: sum(array[$T] of var float);

function     int:   product(array[$T] of     int);
function var int:   product(array[$T] of var int: x) =
  let { array[int] of var int: xx = array1d(1..length(x),x) }
  in product_rec(xx);
function var int:   product_rec(array[int] of var int: x) =
  if length(x)=0 then 1
  elseif length(x)=1 then x[min(index_set(x))]
  else x[min(index_set(x))]*
       product_rec([x[i] | i in min(index_set(x))+1..max(index_set(x))])
  endif;

function     float: product(array[$T] of     float);
function var float: product(array[$T] of var float);

function     $T: min(    $T,     $T);
function var $T: min(var $T, var $T);
function     $T: min(array[$U] of     $T);
function var $T: min(array[$U] of var $T);
function     $T: max(    $T,     $T);
function var $T: max(var $T, var $T);
function     $T: max(array[$U] of     $T);
function var $T: max(array[$U] of var $T);
function int: min(set of int);
function int: max(set of int);

function var int: max(var int: x, var int: y) :: promise_total =
  let { var max(lb(x),lb(y))..max(ub(x),ub(y)): m;
        constraint int_max(x,y,m);
  } in m;

function var int: max_t(array[int] of var int: x) :: promise_total =
  if length(x)=0 then 0
  elseif length(x)=1 then x[min(index_set(x))]
  elseif length(x)=2 then max(x[1],x[2])
  else let {
    var lb_array(x)..ub_array(x): m;
    constraint maximum(m,x);
  } in m
  endif;
function var int: max(array[$U] of var int: x) =
  let {
    array[int] of var int: xx = array1d(1..length(x),x);
    constraint length(x) >= 1;
  } in max_t(xx);

function var int: min(var int: x, var int: y) :: promise_total =
  let { var min(lb(x),lb(y))..min(ub(x),ub(y)): m;
        constraint int_min(x,y,m);
  } in m;

function var int: min_t(array[int] of var int: x) :: promise_total =
  if length(x)=0 then 0
  elseif length(x)=1 then x[1]
  elseif length(x)=2 then min(x[1],x[2])
  else let {
    var lb_array(x)..ub_array(x): m;
    constraint minimum(m,x);
  } in m
  endif;
function var int: min(array[$U] of var int: x) =
  let {
    array[int] of var int: xx = array1d(1..length(x),x);
    constraint length(x) >= 1;
  } in min_t(xx);

% Floating point min and max
% TODO: add bounds reasoning

function var float: max(var float: x, var float: y) :: promise_total =
  let { var float: m;
        constraint float_max(x,y,m);
  } in m;

function var float: max_t(array[int] of var float: x) :: promise_total =
  if length(x)=0 then 0.0
  elseif length(x)=1 then x[min(index_set(x))]
  elseif length(x)=2 then min(x[1],x[2])
  else let {
    var float: m;
    constraint maximum(m,x);
  } in m
  endif;
function var float: max(array[$U] of var float: x) =
  let {
    array[int] of var float: xx = array1d(1..length(x),x);
    constraint length(x) >= 1;
  } in max_t(xx);

function var float: min(var float: x, var float: y) :: promise_total =
  let { var float: m;
        constraint float_min(x,y,m);
  } in m;

function var float: min_t(array[int] of var float: x) :: promise_total =
  if length(x)=0 then 0.0
  elseif length(x)=1 then x[1]
  elseif length(x)=2 then min(x[1],x[2])
  else let {
    var float: m;
    constraint minimum(m,x);
  } in m
  endif;
function var float: min(array[$U] of var float: x) =
  let {
    array[int] of var float: xx = array1d(1..length(x),x);
    constraint length(x) >= 1;
  } in min_t(xx);


function int: abs(int);

function var int: abs(var int: x) :: promise_total =
  let { var 0..max(-lb(x),ub(x)): m;
        constraint int_abs(x,m);
  } in m;


function float: abs(float);
function var float: abs(var float);

function float: sqrt(float);
function var float: sqrt(var float);

function int: pow(int, int);
function float: pow(float, float);

function float: exp(float);
function var float: exp(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_exp(x,r);
  } in r;
function float: ln(float);
function var float: ln(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_ln(x,r);
  } in r;
function float: log10(float);
function var float: log10(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_log10(x,r);
  } in r;
function float: log2(float);
function var float: log2(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_log2(x,r);
  } in r;

function float: log(float, float);

function float: sin(float);
function var float: sin(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_sin(x,r);
  } in r;
function float: cos(float);
function var float: cos(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_cos(x,r);
  } in r;
function float: tan(float);
function var float: tan(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_tan(x,r);
  } in r;
function float: asin(float);
function var float: asin(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_asin(x,r);
  } in r;
function float: acos(float);
function var float: acos(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_acos(x,r);
  } in r;
function float: atan(float);
function var float: atan(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_atan(x,r);
  } in r;
function float: sinh(float);
function var float: sinh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_sinh(x,r);
  } in r;
function float: cosh(float);
function var float: cosh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_cosh(x,r);
  } in r;
function float: tanh(float);
function var float: tanh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_tanh(x,r);
  } in r;
function float: asinh(float);
function var float: asinh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_asinh(x,r);
  } in r;
function float: acosh(float);
function var float: acosh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_acosh(x,r);
  } in r;
function float: atanh(float);
function var float: atanh(var float: x) ::promise_total =
  let {
    var float: r;
    constraint float_atanh(x,r);
  } in r;

% Logical operations
function     bool: '/\'(    bool,     bool);
function var bool: '/\'(var bool, var bool);
function     bool: '\/'(    bool,     bool);
function var bool: '\/'(var bool, var bool);
function     bool: '->'(    bool,     bool);
function var bool: '->'(var bool, var bool);
function     bool: '<-'(    bool,     bool);
function var bool: '<-'(var bool, var bool);
function     bool: '<->'(    bool,     bool);
function var bool: '<->'(var bool, var bool);
function     bool: 'xor'(    bool,     bool);
function var bool: 'xor'(var bool, var bool);
function     bool: 'not'(    bool);
function var bool: 'not'(var bool);
function     bool: forall(array[$T] of     bool);
function var bool: forall(array[$T] of var bool);
function     bool: exists(array[$T] of     bool);
function var bool: exists(array[$T] of var bool);
function     bool: xorall(array[$T] of     bool);
function     bool: iffall(array[$T] of     bool);
function var bool: iffall(array[$T] of var bool);
function var bool: clause(array[$T] of var bool, array[$T] of var bool);
function var bool: clause(array[$T] of     bool, array[$T] of     bool);

% Set operations
function     bool: 'in'(    int,     set of int);
function var bool: 'in'(var int, var set of int);

function     bool: 'subset'(    set of $T,      set of $T);
function var bool: 'subset'(var set of int, var set of int);
function     bool: 'superset'(    set of $T,      set of $T);
function var bool: 'superset'(var set of int, var set of int);

function     set of $T:  'union'(    set of $T,      set of $T);
function var set of int: 'union'(var set of int, var set of int);
function     set of $T:  'intersect'(    set of $T,      set of $T);
function var set of int: 'intersect'(var set of int, var set of int);
function     set of $T:  'diff'(    set of $T,      set of $T);
function var set of int: 'diff'(var set of int, var set of int);
function     set of $T:  'symdiff'(    set of $T,      set of $T);
function var set of int: 'symdiff'(var set of int, var set of int);

function set of int: '..'(int,int);
function set of float: '..'(float,float);

function     int: card(    set of $T);
function var int: card(var set of int);

function     set of $U:  array_union(array[$T] of     set of $U);
function var set of int: array_union(array[$T] of var set of int: x) =
  let { array[int] of var set of int: xx = array1d(1..length(x),x) }
  in array_union_rec(xx);
function var set of int: array_union_rec(array[int] of var set of int: x) =
  if length(x)=0 then {}
  elseif length(x)=1 then x[min(index_set(x))]
  else x[min(index_set(x))] union
       array_union_rec([x[i] | i in min(index_set(x))+1..max(index_set(x))])
  endif;

function     set of $U:  array_intersect(array[$T] of     set of $U);
function var set of int: array_intersect(array[$T] of var set of int: x) =
  let { array[int] of var set of int: xx = array1d(1..length(x),x) }
  in array_intersect_rec(xx);
function var set of int: array_intersect_rec(array[int] of var set of int: x) =
  if length(x)=0 then {}
  elseif length(x)=1 then x[min(index_set(x))]
  else x[min(index_set(x))] intersect
       array_intersect_rec([x[i] | i in min(index_set(x))+1..max(index_set(x))])
  endif;

function set of int: '++'(set of int: x, set of int: y) = x union y;

% Array operations
function int: length(array[$T] of var opt $U);

test index_sets_agree(array[$T] of var opt $U, array[$T] of var opt $W);

function set of int: index_set(array[int] of var opt $U);
function set of int: index_set_1of2(array[int,int] of var opt $U);
function set of int: index_set_2of2(array[int,int] of var opt $U);
function set of int: index_set_1of3(array[int,int,int] of var opt $U);
function set of int: index_set_2of3(array[int,int,int] of var opt $U);
function set of int: index_set_3of3(array[int,int,int] of var opt $U);
function set of int: index_set_1of4(array[int,int,int,int] of var opt $U);
function set of int: index_set_2of4(array[int,int,int,int] of var opt $U);
function set of int: index_set_3of4(array[int,int,int,int] of var opt $U);
function set of int: index_set_4of4(array[int,int,int,int] of var opt $U);
function set of int: index_set_1of5(array[int,int,int,int,int] of var opt $U);
function set of int: index_set_2of5(array[int,int,int,int,int] of var opt $U);
function set of int: index_set_3of5(array[int,int,int,int,int] of var opt $U);
function set of int: index_set_4of5(array[int,int,int,int,int] of var opt $U);
function set of int: index_set_5of5(array[int,int,int,int,int] of var opt $U);
function set of int: index_set_1of6(array[int,int,int,int,int,int] of var opt $U);
function set of int: index_set_2of6(array[int,int,int,int,int,int] of var opt $U);
function set of int: index_set_3of6(array[int,int,int,int,int,int] of var opt $U);
function set of int: index_set_4of6(array[int,int,int,int,int,int] of var opt $U);
function set of int: index_set_5of6(array[int,int,int,int,int,int] of var opt $U);
function set of int: index_set_6of6(array[int,int,int,int,int,int] of var opt $U);

function array[int] of $V: array1d(set of int,
                                   array[$U] of $V);
function array[int] of var $V: array1d(set of int,
                                       array[$U] of var $V);
function array[int] of var opt $V: array1d(set of int,
                                           array[$U] of var opt $V);

function array[int,int] of $V: array2d(set of int, set of int, 
                                       array[$U] of $V);
function array[int,int] of var $V: array2d(set of int, set of int, 
                                           array[$U] of var $V);
function array[int,int] of var opt $V: array2d(set of int, set of int, 
                                               array[$U] of var opt $V);

function array[int,int,int] of $V: array3d(set of int,
                                           set of int,
                                           set of int,
                                           array[$U] of $V);
function array[int,int,int] of var $V: array3d(set of int,
                                               set of int,
                                               set of int,
                                               array[$U] of var $V);
function array[int,int,int] of var opt $V: array3d(set of int,
                                                   set of int,
                                                   set of int,
                                                   array[$U] of var opt $V);

function array[int,int,int,int] of $V: array4d(set of int,
                                               set of int,
                                               set of int,
                                               set of int,
                                               array[$U] of $V);
function array[int,int,int,int] of var $V: array4d(set of int,
                                                   set of int,
                                                   set of int,
                                                   set of int,
                                                   array[$U] of var $V);
function array[int,int,int,int] of var opt $V: array4d(set of int,
                                                       set of int,
                                                       set of int,
                                                       set of int,
                                                       array[$U] of var opt $V);

function array[int,int,int,int,int] of $V: array5d(set of int,
                                                   set of int,
                                                   set of int,
                                                   set of int,
                                                   set of int,
                                                   array[$U] of $V);
function array[int,int,int,int,int] of var $V: array5d(set of int,
                                                       set of int,
                                                       set of int,
                                                       set of int,
                                                       set of int,
                                                       array[$U] of var $V);
function array[int,int,int,int,int] of var opt $V: array5d(set of int,
                                                           set of int,
                                                           set of int,
                                                           set of int,
                                                           set of int,
                                                           array[$U] of var opt $V);

function array[int,int,int,int,int,int] of $V: array6d(set of int,
                                                       set of int,
                                                       set of int,
                                                       set of int,
                                                       set of int,
                                                       set of int,
                                                       array[$U] of $V);
function array[int,int,int,int,int,int] of var $V: array6d(set of int,
                                                           set of int,
                                                           set of int,
                                                           set of int,
                                                           set of int,
                                                           set of int,
                                                           array[$U] of var $V);
function array[int,int,int,int,int,int] of var opt $V: array6d(set of int,
                                                               set of int,
                                                               set of int,
                                                               set of int,
                                                               set of int,
                                                               set of int,
                                                               array[$U] of var opt $V);

% Coercion operations

function int: ceil(float);
function int: floor(float);
function int: round(float);

function int: bool2int(bool);
function var int: bool2int(var bool);
function float: int2float(int);
function var float: int2float(var int: x) =
  let { var int2float(lb(x))..int2float(ub(x)): y;
        constraint int2float(x,y); } in y;
predicate int2float(var int, var float);

% Only supported for set of int:
% function array[int] of $T: set2array(set of $T);
function array[int] of int: set2array(set of int);

% String operations

function string: show(var opt $T);
function string: show(array[$U] of var opt $T);
function string: show_int(int, var int);
function string: show_float(int, int, var float);
function string: '++'(string, string);
function string: concat(array[$T] of string);
function string: join(string, array[$T] of string);

% Bound and domain operations

function int: lb(var int);
function int: ub(var int);
function int: lb(var opt int);
function int: ub(var opt int);
function float: lb(var float);
function float: ub(var float);
function set of int: lb(var set of int);
function set of int: ub(var set of int);
function array[$U] of int: lb(array[$U] of var int);
function array[$U] of int: ub(array[$U] of var int);
function array[$U] of float: lb(array[$U] of var float);
function array[$U] of float: ub(array[$U] of var float);
function array[$U] of set of int: lb(array[$U] of var set of int);
function array[$U] of set of int: ub(array[$U] of var set of int);
function int: lb_array(array[$U] of var opt int);
function int: ub_array(array[$U] of var opt int);
function float: lb_array(array[$U] of var float);
function float: ub_array(array[$U] of var float);
function set of int: lb_array(array[$U] of var set of int);
function set of int: ub_array(array[$U] of var set of int);
function set of int: dom(var int);
function set of int: dom_array(array[$T] of var int);
function int: dom_size(var int: x) = card(dom(x));

function par bool: has_bounds(var int);
function par bool: has_ub_set(var set of int);

% Other operations

function $T: assert(bool, string, $T);
function var $T: assert(bool, string, var $T);
function var opt $T: assert(bool, string, var opt $T);
function bool: assert(bool, string);
function $T: trace(string, $T);
function var $T: trace(string, var $T);
function var opt $T: trace(string, var opt $T);
function $T: fix(var opt $T);
function array[$U] of $T: fix(array[$U] of var opt $T);
function bool: is_fixed(var opt $T);
function bool: is_fixed(array[$U] of var opt $T);

function bool: abort(string);

function array[int] of var $T: sort_by(array[int] of var $T, array[int] of int);
function array[int] of int: sort(array[int] of int);
function array[int] of float: sort(array[int] of float);
function array[int] of bool: sort(array[int] of bool);
