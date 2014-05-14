%
% File: minizinc/redefinitions.mzn
%
% Main authors:
%   Thibaut Feydy <tfeydy@csse.unimelb.edu.au>
%
% Copyright:
%   2010-2011 NICTA
%
%------------------------------------------------------------------------------
% CPX redefinitions file
%------------------------------------------------------------------------------

%------------------------------------------------------------------------------
% CPX builtins declarations.
%------------------------------------------------------------------------------

% TODO
predicate array_bool_odd(array[int] of var bool : bs);

% TODO
predicate array_bool_even(array[int] of var bool : bs);

predicate bool2int(var bool : a, var int : b);

predicate bool_clause(
  array[int] of var bool : pos, array[int] of var bool : neg);

predicate bool_clause_reif(
  array[int] of var bool : pos, array[int] of var bool : neg, var bool : res);

predicate bool_conj_reif(
  array[int] of var bool : pos, array[int] of var bool : neg, var bool : res);

predicate int_in(var int : x, par set of int : s);

predicate int_lin_eq(
  array[int] of int : cs, array[int] of var int : vs, int : val);

predicate int_lin_le(
  array[int] of int : cs, array[int] of var int : vs, int : val);

predicate int_lin_ne(
  array[int] of int : cs, 
  array[int] of var int : vs, 
  int : val);

predicate int_lin_eq_reif(
  array[int] of int : cs, 
  array[int] of var int : vs, 
  int : val,
  var bool : res);

predicate int_lin_le_reif(
  array[int] of int : cs, 
  array[int] of var int : vs, 
  int : val,
  var bool : res);

predicate int_lin_ne_reif(
  array[int] of int : cs, 
  array[int] of var int : vs, 
  int : val,
  var bool : res);

%------------------------------------------------------------------------------
% Array Standard Predicates Redefinitions
%------------------------------------------------------------------------------

predicate array_bool_and(array[int] of var bool : arr, var bool : res) =
  bool_conj_reif(arr, [], res);

predicate array_bool_or(array[int] of var bool: as, var bool : r) =
  if is_fixed(r) /\ fix(r) then bool_clause(as, []) else bool_clause_reif(as, [], r) endif;

predicate array_bool_xor(array[int] of var bool : arr) = 
  array_bool_odd(arr);

% predicate array_float_element : unsupported

% predicate array_var_float_element : unsupported

%------------------------------------------------------------------------------
% Boolean Standard Predicates Redefinitions
%------------------------------------------------------------------------------

predicate bool_and(var bool: a, var bool : b, var bool : r) = 
  bool_conj_reif([a, b], [], r);

predicate bool_eq(var bool : a, var bool : b) =
  array_bool_even([a, b]);

predicate bool_eq_reif(var bool : a, var bool : b, var bool : r) = 
  array_bool_odd([a, b, r]);

predicate bool_le(var bool: a, var bool: b) =
  bool_clause([b], [a]);

predicate bool_le_reif(var bool: a, var bool: b, var bool: r) =
  bool_clause_reif([b], [a], r);

% Seriously ? What a useless constraint
predicate bool_lt(var bool: a, var bool: b) =
  not(a) /\ b;

predicate bool_lt_reif(var bool: a, var bool: b, var bool: r) =
  bool_conj_reif([b], [a], r);

predicate bool_not(var bool: a, var bool: b) = 
  array_bool_odd([a, b]);

predicate bool_or(var bool: a, var bool: b, var bool: r) =
  if is_fixed(r) /\ fix(r) then bool_clause([a, b], []) else bool_clause_reif([a, b], [], r) endif;

predicate bool_xor(var bool: a, var bool: b, var bool: r) =
  array_bool_even([a, b, r]);

%------------------------------------------------------------------------------
% Float Standard Predicates Redefinitions
%------------------------------------------------------------------------------

% all float predicates : unsupported

%------------------------------------------------------------------------------
% Integer Standard Predicates Redefinitions
%------------------------------------------------------------------------------

predicate int_eq(var int: x, var int: y) = 
  int_lin_eq([1, -1], [x, y], 0);

predicate int_eq_reif(var int: x, var int: y, var bool: res) = 
  int_lin_eq_reif([1, -1], [x, y], 0, res);

predicate int_le(var int: x, var int: y) = 
  int_lin_le([1, -1], [x, y], 0);

predicate int_le_reif(var int: x, var int: y, var bool : res) = 
  int_lin_le_reif([1, -1], [x, y], 0, res);

predicate int_lt(var int: x, var int: y) = 
  int_lin_le([1, -1], [x, y], -1);

predicate int_lt_reif(var int: x, var int: y, var bool : res) = 
  int_lin_le_reif([1, -1], [x, y], -1, res);

predicate int_ne(var int: x, var int: y) = 
  int_lin_ne([1, -1], [x, y], 0);

predicate int_ne_reif(var int: x, var int: y, var bool : res) = 
  int_lin_ne_reif([1, -1], [x, y], 0, res);

predicate int_plus(var int: x, var int: y, var int: z) =
  int_lin_eq([1, 1, -1], [x, y, z], 0);

%------------------------------------------------------------------------------
% Sets Standard Predicates Redefinitions
%------------------------------------------------------------------------------

