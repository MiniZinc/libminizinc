/***
  @groupdef stdlib.optiontypes Option type support

  These functions and predicates implement the standard library for working
  with option types. Note that option type support is still incomplete.
*/

/** @group stdlib.optiontypes Return value of \a x if \a x is not absent. Returns 
  undefined when evaluated on absent value. */
function $$T: deopt(opt $$T: x);
/** @group stdlib.optiontypes Return value of \a x if \a x is not absent. Returns
  undefined when evaluated on absent value. */
function $T: deopt(opt $T: x);
/** @group stdlib.optiontypes Return array of the value of \a x[i] where \a x[i] is not absent.
  Returns undefined when evaluated with an absent element. */
function array [$$U] of $$T: deopt(array [$$U] of opt $$T: x) = array1d(index_set(x), [deopt(x[i]) | i in index_set(x)]);
/** @group stdlib.optiontypes Return array of the value of \a x[i] where \a x[i] is not absent.
  Returns undefined when evaluated with an absent element. */
function array [$$U] of $T: deopt(array [$$U] of opt $T: x) = array1d(index_set(x), [deopt(x[i]) | i in index_set(x)]);

/** @group stdlib.optiontypes Return value \a x unchanged (since \a x is guaranteed
  to be non-optional). */
function var $T: deopt(var $T: x) = x;
/** @group stdlib.optiontypes Return value \a x unchanged (since \a x is guaranteed
  to be non-optional). */
function $T: deopt($T: x) = x;

/** @group stdlib.optiontypes Return value \a x unchanged (since \a x is guaranteed
  to be non-optional). */
function array [$U] of var $T: deopt(array [$U] of var $T: x) = x;
/** @group stdlib.optiontypes Return array of the value of \a x[i] (assumes that \a x[i] is not absent). */
function array [$$U] of var bool : deopt(array [$$U] of var opt bool : x) :: promise_total =
  array1d(index_set(x), [deopt(x[i]) | i in index_set(x)]);
/** @group stdlib.optiontypes Return array of the value of \a x[i] (assumes that \a x[i] is not absent). */
function array [$$U] of var $$E : deopt(array [$$U] of var opt $$E : x) :: promise_total =
  array1d(index_set(x), [deopt(x[i]) | i in index_set(x)]);
/** @group stdlib.optiontypes Return array of the value of \a x[i] (assumes that \a x[i] is not absent). */
function array [$$U] of var float : deopt(array [$$U] of var opt float : x) :: promise_total =
  array1d(index_set(x), [deopt(x[i]) | i in index_set(x)]);

/** @group stdlib.optiontypes Test if \a x is not absent (always returns true) */
test occurs($T: x) = true;
/** @group stdlib.optiontypes Test if \a x is not absent (always returns true) */
test occurs(var $T: x) = true;
/** @group stdlib.optiontypes Test if \a x is not absent (always returns true) */
test occurs(set of $$T: x) = true;
/** @group stdlib.optiontypes Test if \a x is not absent (always returns true) */
test occurs(var set of $$T: x) = true;

/** @group stdlib.optiontypes Test if \a x is not absent */
test occurs(opt $T: x);
/** @group stdlib.optiontypes Test if \a x is absent (always returns false) */
test absent($T: x) = false;
/** @group stdlib.optiontypes Test if \a x is absent (always returns false) */
test absent(var $T: x) = false;
/** @group stdlib.optiontypes Test if \a x is absent (always returns false) */
test absent(set of $$T: x) = false;
/** @group stdlib.optiontypes Test if \a x is absent (always returns false) */
test absent(var set of $$T: x) = false;
/** @group stdlib.optiontypes Test if \a x is absent */
test absent(opt $T: x) = not occurs(x);

/***
  @groupdef stdlib.optiontypes.bool Option type support for Booleans
*/

/** @group stdlib.optiontypes.bool True iff \a x is not absent */
function var bool : occurs(var opt bool: x) ::promise_total = occurs_bool(x);

function bool : occurs_bool(opt bool: x) ::promise_total = occurs(x);
function bool : occurs_bool(var bool: x) ::promise_total = true;
function var bool : occurs_bool(var opt bool: x) ::promise_total =
  let {
    any: xx = opt_internal_bool(x);
    any: b = xx.1;
    any: dx = xx.2;
    constraint (x = reverse_map_var_opt(b, dx)) :: is_reverse_map;
  } in xx.1;
  
/** @group stdlib.optiontypes.bool Return value of \a x (assumes that \a x is not absent) */
function var bool : deopt(var opt bool : x) ::promise_total = deopt_bool(x);

function bool : deopt_bool(opt bool : x) ::promise_total = deopt(x);
function var bool : deopt_bool(var bool : x) ::promise_total = x;
function var bool : deopt_bool(var opt bool : x) ::promise_total =
  let {
    any: xx = opt_internal_bool(x);
    any: b = xx.1;
    any: dx = xx.2;
    constraint (x = reverse_map_var_opt(b, dx)) :: is_reverse_map;
  } in xx.2;

/** @group stdlib.optiontypes.bool True iff \a x is absent */
predicate absent(var opt bool: x) = not occurs(x);

function tuple(var bool, var bool) : opt_internal_bool(var opt bool: x) ::promise_total =
  let {
    var bool: b;
    var bool: y;
  } in (b, y);

predicate mzn_reverse_map_var(var opt bool: x) =
  let {
    any: xx = opt_internal_bool(x);
    any: b = xx.1;
    any: dx = xx.2;
    constraint (x = reverse_map_var_opt(b, dx)) :: is_reverse_map;
  } in true;

/***
  @groupdef stdlib.optiontypes.int Option type support for integers
*/

/** @group stdlib.optiontypes.int True iff \a x is not absent */
function var bool : occurs(var opt int : x) ::promise_total = occurs_int(x);

function bool : occurs_int(opt int : x) ::promise_total = occurs(x);
function bool : occurs_int(var int : x) ::promise_total = true;
function var bool : occurs_int(var opt int : x) ::promise_total =
  let {
    any: xx = opt_internal_int(x);
    any: b = xx.1;
    any: dx = xx.2;
    constraint (x = reverse_map_var_opt(b, dx)) :: is_reverse_map;
  } in xx.1;
function var bool : occurs_int(var opt bool : x) ::promise_total = occurs(x);
function bool : occurs_int(opt bool : x) ::promise_total = occurs(x);
function bool : occurs_int(var bool : x) ::promise_total = true;

/** @group stdlib.optiontypes.int Return value of \a x (assumes that \a x is not absent) */
function var $$E : deopt(var opt $$E : x) ::promise_total = deopt_int(x);

function int : deopt_int(opt int : x) ::promise_total = deopt(x);
function var int : deopt_int(var int : x) ::promise_total = x;
function var int : deopt_int(var opt int : x) ::promise_total =
  let {
    any: xx = opt_internal_int(x);
    any: b = xx.1;
    any: dx = xx.2;
    constraint (x = reverse_map_var_opt(b, dx)) :: is_reverse_map;
  } in xx.2;
function var bool : deopt_int(var opt bool : x) ::promise_total = deopt(x);
function var bool : deopt_int(var bool : x) ::promise_total = x;
function bool : deopt_int(opt bool : x) ::promise_total = deopt(x);


/** @group stdlib.optiontypes.int True iff \a x is absent */
function var bool: absent(var opt int: x) ::promise_total = not occurs(x);

/** @group stdlib.optiontypes.int True if \a x had zero in its original domain.

    Returns true if absent zero representation is disabled or if it is possible
    that \( \text{occurs}(x) \wedge \text{deopt}(x) = 0 \) */
function bool: had_zero(var opt int: x) :: cache_result = not mzn_check_absent_zero() \/ (0 in dom(x));
/** @group stdlib.optiontypes.int True if \a x had zero in its original domain.

    Returns true if absent zero representation is disabled or if it is possible
    that \( \text{occurs}(x) \wedge \text{deopt}(x) = 0 \) */
function bool: had_zero(opt int: x) = not mzn_check_absent_zero() \/ (x = 0);
/** @group stdlib.optiontypes.int True if \a x had zero in its original domain.

    Returns true if absent zero representation is disabled or if it is possible
    that \( \text{occurs}(x) \wedge \text{deopt}(x) = 0 \) */
function bool: had_zero(array [int] of var opt int: x) = exists (xi in x) (had_zero(xi));

function tuple(var bool, var int): opt_internal_int(var opt int: x) ::promise_total =
  if dom(x) = {} then
    (false, 0)
  else 
    let {
      var bool: b;
      var dom(x) union if mzn_check_absent_zero() then {0} else {} endif: y;
      constraint if mzn_check_absent_zero() then
        if had_zero(x) then
          b \/ y = 0
        else
          b <-> y != 0
        endif
      endif;
    } in (b, y)
  endif;

predicate mzn_reverse_map_var(var opt int: x) =
  let {
    any: xx = opt_internal_int(x);
    any: b = xx.1;
    any: dx = xx.2;
    constraint (x = reverse_map_var_opt(b, dx)) :: is_reverse_map;
  } in true;

/* Internal function used to optimize over option type objective */

function var int: objective_deopt_(var opt int: x, bool: direction) =
  let {
    int: worst = if direction then lb(x)-1 else ub(x)+1 endif;
  } in if occurs(x) then deopt(x) else worst endif;

/***
  @groupdef stdlib.optiontypes.float Option type support for floats
*/

/** @group stdlib.optiontypes.float True iff \a x is not absent */
function var bool : occurs(var opt float : x) ::promise_total = occurs_float(x);

function bool : occurs_float(opt float : x) ::promise_total = occurs(x);
function bool : occurs_float(opt int : x) ::promise_total = occurs(x);
function bool : occurs_float(opt bool : x) ::promise_total = occurs(x);
function bool : occurs_float(var float : x) ::promise_total = true;
function var bool : occurs_float(var opt float : x) ::promise_total =
  let {
    any: xx = opt_internal_float(x);
    any: b = xx.1;
    any: dx = xx.2;
    constraint (x = reverse_map_var_opt(b, dx)) :: is_reverse_map;
  } in xx.1;
function var bool : occurs_float(var opt int : x) ::promise_total = occurs_int(x);
function bool : occurs_float(var int : x) ::promise_total = true;
function var bool : occurs_float(var opt bool : x) ::promise_total = occurs_bool(x);
function bool : occurs_float(var bool : x) ::promise_total = true;


/** @group stdlib.optiontypes.float Return value of \a x (assumes that \a x is not absent) */
function var float : deopt(var opt float : x) ::promise_total = deopt_float(x);

function var float : deopt_float(var float : x) ::promise_total = x;
function var float : deopt_float(var opt float : x) ::promise_total =
  let {
    any: xx = opt_internal_float(x);
    any: b = xx.1;
    any: dx = xx.2;
    constraint (x = reverse_map_var_opt(b, dx)) :: is_reverse_map;
  } in xx.2;
function var int : deopt_float(var opt int : x) ::promise_total = deopt_int(x);
function var int : deopt_float(var int : x) ::promise_total = x;
function var bool : deopt_float(var opt bool : x) ::promise_total = deopt_bool(x);
function var bool : deopt_float(var bool : x) ::promise_total = x;
function float : deopt_float(opt float : x) ::promise_total = deopt(x);
function int : deopt_float(opt int : x) ::promise_total = deopt(x);
function bool : deopt_float(opt bool : x) ::promise_total = deopt(x);

/** @group stdlib.optiontypes.float True iff \a x is absent */
function var bool: absent(var opt float: x) ::promise_total = not occurs(x);

function tuple(var bool, var float): opt_internal_float(var opt float: x) ::promise_total =
  let {
    var bool: b;
    var lb(x)..ub(x): y;
  } in (b, y);

predicate mzn_reverse_map_var(var opt float: x) =
  let {
    any: xx = opt_internal_float(x);
    any: b = xx.1;
    any: dx = xx.2;
    constraint (x = reverse_map_var_opt(b, dx)) :: is_reverse_map;
  } in true;

function var opt $T: reverse_map_var_opt(var bool: b, var $T: dx) ::output_only;
function opt $T: reverse_map_var_opt(bool: occ, $T: dx) ::promise_total =
  if occ then dx else <> endif;
