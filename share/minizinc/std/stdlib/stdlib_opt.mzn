/***
  @groupdef stdlib.optiontypes Option type support

  These functions and predicates implement the standard library for working
  with option types. Note that option type support is still incomplete.
*/

/** @group stdlib.optiontypes Return value of \a x if \a x is not absent. Aborts 
  when evaluated on absent value. */
function $T: deopt(opt $T: x);
/** @group stdlib.optiontypes Return value of \a x if \a x is not absent. Aborts 
  when evaluated on absent value. */
function $$T: deopt(opt $$T: x);

/** @group stdlib.optiontypes Return value \a x unchanged (since \a x is guaranteed
  to be non-optional). */
function var $T: deopt(var $T: x) = x;

/** @group optiontypes Test if \a x is not absent (always returns true) */
test occurs(var $T: x) = true;
/** @group optiontypes Test if \a x is not absent */
test occurs(opt $T: x);
/** @group optiontypes Test if \a x is absent (always returns false) */
test absent(var $T: x) = false;
/** @group optiontypes Test if \a x is absent */
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
    var bool : b = occurs_internal_bool(x);
    var bool : dx = deopt_internal_bool(x);
    constraint (x = reverse_map_var_opt_bool(b,dx)) :: is_reverse_map;
  } in b;
  
/** @group stdlib.optiontypes.bool Return value of \a x (assumes that \a x is not absent) */
function var bool : deopt(var opt bool : x) ::promise_total = deopt_bool(x);

function bool : deopt_bool(opt bool : x) ::promise_total = deopt(x);
function var bool : deopt_bool(var bool : x) ::promise_total = x;
function var bool : deopt_bool(var opt bool : x) ::promise_total =
  let {
    var bool : b = occurs_internal_bool(x);
    var bool : dx = deopt_internal_bool(x);
    constraint (x = reverse_map_var_opt_bool(b,dx)) :: is_reverse_map;
  } in dx;

/** @group stdlib.optiontypes.bool True iff \a x is absent */
predicate absent(var opt bool: x) = not occurs(x);

function var bool: occurs_internal_bool(var opt bool: x) ::promise_total =
  let { var bool : b; } in b;
function var bool : deopt_internal_bool(var opt bool : x) ::promise_total =
  let { var bool: y } in y;

function var opt bool: reverse_map_var_opt_bool(var bool: occ, var bool: d);
function opt bool: reverse_map_var_opt_bool(bool: occ, bool: d) ::promise_total =
  if occ then d else <> endif;

predicate mzn_reverse_map_var(var opt bool: x) =
  let {
    var bool : b = occurs_internal_bool(x);
    var bool : dx = deopt_internal_bool(x);
    constraint (x = reverse_map_var_opt_bool(b,dx)) :: is_reverse_map;
  } in true;

/** @group stdlib.optiontypes.bool Weak equality. True if either \a x or \a y are absent, or
present and equal.*/
function var bool: '~='(var opt bool: x, var opt bool: y) ::promise_total =
    absent(x) \/ absent(y) \/ deopt(x)=deopt(y);

/***
  @groupdef stdlib.optiontypes.int Option type support for integers
*/

/** @group stdlib.optiontypes.int True iff \a x is not absent */
function var bool : occurs(var opt int : x) ::promise_total = occurs_int(x);

function bool : occurs_int(opt int : x) ::promise_total = occurs(x);
function bool : occurs_int(var int : x) ::promise_total = true;
function var bool : occurs_int(var opt int : x) ::promise_total =
  let {
    var bool : b = occurs_internal_int(x);
    var int  : dx = deopt_internal_int(x);
    constraint (x = reverse_map_var_opt_int(b,dx)) :: is_reverse_map;
  } in b;
function var bool : occurs_int(var opt bool : x) ::promise_total = occurs(x);
function bool : occurs_int(opt bool : x) ::promise_total = occurs(x);
function bool : occurs_int(var bool : x) ::promise_total = true;

/** @group stdlib.optiontypes.int Return value of \a x (assumes that \a x is not absent) */
function var $$E : deopt(var opt $$E : x) ::promise_total = deopt_int(x);

function int : deopt_int(opt int : x) ::promise_total = deopt(x);
function var int : deopt_int(var int : x) ::promise_total = x;
function var int : deopt_int(var opt int : x) ::promise_total =
  let {
    var bool : b = occurs_internal_int(x);
    var int  : dx = deopt_internal_int(x);
    constraint (x = reverse_map_var_opt_int(b,dx)) :: is_reverse_map;
  } in dx;
function var bool : deopt_int(var opt bool : x) ::promise_total = deopt(x);
function var bool : deopt_int(var bool : x) ::promise_total = x;
function bool : deopt_int(opt bool : x) ::promise_total = deopt(x);


/** @group stdlib.optiontypes.int True iff \a x is absent */
function var bool: absent(var opt int: x) ::promise_total = not occurs(x);

function var bool: occurs_internal_int(var opt int: x) ::promise_total =
  let { var bool : b; } in b;
function var int : deopt_internal_int(var opt int : x) ::promise_total =
  let { var dom(x): y } in y;

function var opt int: reverse_map_var_opt_int(var bool: occ, var int: d);
function opt int: reverse_map_var_opt_int(bool: occ, int: d) ::promise_total =
  if occ then d else <> endif;

predicate mzn_reverse_map_var(var opt int: x) =
  let {
    var bool : b = occurs_internal_int(x);
    var int : dx = deopt_internal_int(x);
    constraint (x = reverse_map_var_opt_int(b,dx)) :: is_reverse_map;
  } in true;


/** @group stdlib.optiontypes.int Weak addition. Return sum of \a x and \a y if both
are present, otherwise return absent. */
function var opt int: '~+'(var opt int: x, var opt int: y) ::promise_total =
    let { 
    int: l = if lb(x)=-infinity \/ lb(y)=-infinity then -infinity else lb(x)+lb(y) endif;
    int: u = if ub(x)=infinity \/ ub(y)=infinity then infinity else ub(x)+ub(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)+deopt(y);
    } in result;

/** @group stdlib.optiontypes.int Weak subtraction. Return difference of \a x and \a y if both
are present, otherwise return absent. */
function var opt int: '~-'(var opt int: x, var opt int: y) ::promise_total =
    let { 
    int: l = if lb(x)=-infinity \/ ub(y)=infinity then -infinity else lb(x)-ub(y) endif;
    int: u = if ub(x)=infinity \/ lb(y)=-infinity then infinity else ub(x)-lb(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)-deopt(y);
    } in result;

/** @group stdlib.optiontypes.int Weak multiplication. Return product of \a x and \a y if both
are present, otherwise return absent. */
function var opt int: '~*'(var opt int: x, var opt int: y) ::promise_total =
    if absent(x) \/ absent(y) then <>
    else deopt(x)*deopt(y) endif;

/** @group stdlib.optiontypes.int Weak equality. True if either \a x or \a y are absent, or
present and equal.*/
function var bool: '~='(var opt int: x, var opt int: y) ::promise_total =
    absent(x) \/ absent(y) \/ deopt(x)=deopt(y);

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
    var bool : b = occurs_internal_float(x);
    var float : dx = deopt_internal_float(x);
    constraint (x = reverse_map_var_opt_float(b,dx)) :: is_reverse_map;
  } in b;
function var bool : occurs_float(var opt int : x) ::promise_total = occurs_int(x);
function bool : occurs_float(var int : x) ::promise_total = true;
function var bool : occurs_float(var opt bool : x) ::promise_total = occurs_bool(x);
function bool : occurs_float(var bool : x) ::promise_total = true;


/** @group stdlib.optiontypes.float Return value of \a x (assumes that \a x is not absent) */
function var float : deopt(var opt float : x) ::promise_total = deopt_float(x);

function var float : deopt_float(var float : x) ::promise_total = x;
function var float : deopt_float(var opt float : x) ::promise_total =
  let {
    var bool : b = occurs_internal_float(x);
    var float : dx = deopt_internal_float(x);
    constraint (x = reverse_map_var_opt_float(b,dx)) :: is_reverse_map;
  } in dx;
function var int : deopt_float(var opt int : x) ::promise_total = deopt_int(x);
function var int : deopt_float(var int : x) ::promise_total = x;
function var bool : deopt_float(var opt bool : x) ::promise_total = deopt_bool(x);
function var bool : deopt_float(var bool : x) ::promise_total = x;
function float : deopt_float(opt float : x) ::promise_total = deopt(x);
function int : deopt_float(opt int : x) ::promise_total = deopt(x);
function bool : deopt_float(opt bool : x) ::promise_total = deopt(x);

/** @group stdlib.optiontypes.float True iff \a x is absent */
function var bool: absent(var opt float: x) ::promise_total = not occurs(x);

function var bool: occurs_internal_float(var opt float: x) ::promise_total =
  let { var bool : b; } in b;
function var float : deopt_internal_float(var opt float : x) ::promise_total =
  let { var lb(x)..ub(x): y } in y;

function var opt float: reverse_map_var_opt_float(var bool: occ, var float: d); % :NOTE: Must define in cpp?
function opt float: reverse_map_var_opt_float(bool: occ, float: d) ::promise_total =
  if occ then d else <> endif;

predicate mzn_reverse_map_var(var opt float: x) =
  let {
    var bool : b = occurs_internal_float(x);
    var float : dx = deopt_internal_float(x);
    constraint (x = reverse_map_var_opt_float(b,dx)) :: is_reverse_map;
  } in true;

/** @group stdlib.optiontypes.float Weak addition. Return sum of \a x and \a y if both
are present, otherwise return absent. */
function var opt float: '~+'(var opt float: x, var opt float: y) ::promise_total =
    let { 
    float: l = if lb(x)=-infinity \/ lb(y)=-infinity then -infinity else lb(x)+lb(y) endif;
    float: u = if ub(x)=infinity \/ ub(y)=infinity then infinity else ub(x)+ub(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)+deopt(y);
    } in result;

/** @group stdlib.optiontypes.float Weak subtraction. Return difference of \a x and \a y if both
are present, otherwise return absent. */
function var opt float: '~-'(var opt float: x, var opt float: y) ::promise_total =
    let { 
    float: l = if lb(x)=-infinity \/ ub(y)=infinity then -infinity else lb(x)-ub(y) endif;
    float: u = if ub(x)=infinity \/ lb(y)=-infinity then infinity else ub(x)-lb(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)-deopt(y);
    } in result;

/** @group stdlib.optiontypes.float Weak multiplication. Return product of \a x and \a y if both
are present, otherwise return absent. */
function var opt float: '~*'(var opt float: x, var opt float: y) ::promise_total =
    if absent(x) \/ absent(y) then <>
    else deopt(x)*deopt(y) endif;

/** @group stdlib.optiontypes.float Weak equality. True if either \a x or \a y are absent, or
present and equal.*/
function var bool: '~='(var opt float: x, var opt float: y) ::promise_total =
    absent(x) \/ absent(y) \/ deopt(x)=deopt(y);

