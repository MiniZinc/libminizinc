/***
  @groupdef stdlib.builtins.enum Functions for enums
*/

%TODO: Document these
function set of int: anon_enum(int: n) = 1..n;

function set of int: anon_enum_set(set of int: S) = 1..card(S);

function set of int: anon_enum(array[int] of string: x) ::mzn_internal_representation;

/** @group stdlib.builtins.enum Return enum set of \a x */
function set of $$E: enum_of(var opt $$E: x) ::mzn_internal_representation;
/** @group stdlib.builtins.enum Return enum set of \a x */
function set of $$E: enum_of(var set of $$E: x) ::mzn_internal_representation;
/** @group stdlib.builtins.enum Return enum set of \a x */
function set of $$E: enum_of(array[$T] of var opt $$E: x) ::mzn_internal_representation;
/** @group stdlib.builtins.enum Return enum set of \a x */
function set of $$E: enum_of(array[$T] of var set of $$E: x) ::mzn_internal_representation;

function set of $$E: enum_of_internal(set of $$E: e) = e;

/** @group stdlib.builtins.enum Return next greater enum value of \a x */
function $$E: enum_next($$E: x) = enum_next(enum_of(x), x);
/** @group stdlib.builtins.enum Return next greater enum value of \a x */
function opt $$E: enum_next(opt $$E: x) = enum_next(enum_of(x), x);
/** @group stdlib.builtins.enum Return next greater enum value of \a x */
function var $$E: enum_next(var $$E: x) = enum_next(enum_of(x), x);
/** @group stdlib.builtins.enum Return next greater enum value of \a x */
function var opt $$E: enum_next(var opt $$E: x) = enum_next(enum_of(x), x);

/** @group stdlib.builtins.enum Return next greater enum value of \a x in enum \a e */
function $$E: enum_next(set of $$E: e, $$E: x);
/** @group stdlib.builtins.enum Return next greater enum value of \a x in enum \a e */
function opt $$E: enum_next(set of $$E: e, opt $$E: x) =
  if occurs(x) then enum_next(e,deopt(x)) else <> endif;
/** @group stdlib.builtins.enum Return next greater enum value of \a x in enum \a e */
function var $$E: enum_next(set of $$E: e, var $$E: x) =
  let { constraint x < max(e) } in x+1;
/** @group stdlib.builtins.enum Return next greater enum value of \a x in enum \a e */
function var opt $$E: enum_next(set of $$E: e, var opt $$E: x) =
  if occurs(x) then enum_next(e,deopt(x)) else <> endif;

/** @group stdlib.builtins.enum Return next smaller enum value of \a x */
function $$E: enum_prev($$E: x) = enum_prev(enum_of(x), x);
/** @group stdlib.builtins.enum Return next smaller enum value of \a x */
function opt $$E: enum_prev(opt $$E: x) = enum_prev(enum_of(x), x);
/** @group stdlib.builtins.enum Return next smaller enum value of \a x */
function var $$E: enum_prev(var $$E: x) = enum_prev(enum_of(x), x);
/** @group stdlib.builtins.enum Return next smaller enum value of \a x */
function var opt $$E: enum_prev(var opt $$E: x) = enum_prev(enum_of(x), x);

/** @group stdlib.builtins.enum Return next smaller enum value of \a x in enum \a e */
function $$E: enum_prev(set of $$E: e, $$E: x);
/** @group stdlib.builtins.enum Return next smaller enum value of \a x in enum \a e */
function opt $$E: enum_prev(set of $$E: e, opt $$E: x) =
  if occurs(x) then enum_prev(e,deopt(x)) else <> endif;
/** @group stdlib.builtins.enum Return next smaller enum value of \a x in enum \a e */
function var $$E: enum_prev(set of $$E: e, var $$E: x) =
  let { constraint x > min(e) } in x-1;
/** @group stdlib.builtins.enum Return next smaller enum value of \a x in enum \a e */
function var opt $$E: enum_prev(set of $$E: e, var opt $$E: x) =
  if occurs(x) then enum_prev(e,deopt(x)) else <> endif;

/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function $$E: to_enum(set of $$E: X, int: x);
/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function opt $$E: to_enum(set of $$E: X, opt int: x) =
  if occurs(x) then to_enum(X, deopt(x)) else <> endif;
/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function var $$E: to_enum(set of $$E: X, var int: x) =
  let { constraint x in X } in x;
/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function var opt $$E: to_enum(set of $$E: X, var opt int: x) =
  if occurs(x) then to_enum(X, deopt(x)) else <> endif;
  
/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function array[$U] of $$E: to_enum(set of $$E: X, array[$U] of int: x) =
  let { array[int] of int: xx = array1d(x) } in
  arrayXd(x, [ to_enum(X,xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function array[$U] of opt $$E: to_enum(set of $$E: X, array[$U] of opt int: x) =
  let { array[int] of opt int: xx = array1d(x) } in
  arrayXd(x, [ to_enum(X,xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function array[$U] of var $$E: to_enum(set of $$E: X, array[$U] of var int: x) =
  let { array[int] of var int: xx = array1d(x) } in
  arrayXd(x, [ to_enum(X,xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function array[$U] of var opt $$E: to_enum(set of $$E: X, array[$U] of var opt int: x) =
  let { array[int] of var opt int: xx = array1d(x) } in
  arrayXd(x, [ to_enum(X,xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function set of $$E: to_enum(set of $$E: X, set of int: x) = { to_enum(X,i) | i in x };

%/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function var set of $$E: to_enum(set of $$E: X, var set of int: x) = 
  let { var set of X: y; constraint x subset X; constraint forall (i in X) (i in x <-> i in y); } in y;

/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function array[$U] of set of $$E: to_enum(set of $$E: X, array[$U] of set of int: x) =
  let { array[int] of set of int: xx = array1d(x) } in
  arrayXd(x, [ to_enum(X, xx[i]) | i in index_set(xx)]);

/** @group stdlib.builtins.enum Convert \a x to enum type \a X */
function array[$U] of var set of $$E: to_enum(set of $$E: X, array[$U] of var set of int: x) =
  let { array[int] of var set of int: xx = array1d(x) } in
  arrayXd(x, [ to_enum(X, xx[i]) | i in index_set(xx)]);
