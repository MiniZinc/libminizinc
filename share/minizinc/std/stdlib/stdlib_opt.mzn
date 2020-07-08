/***
  @groupdef stdlib.optiontypes Option type support

  These functions and predicates implement the standard library for working
  with option types. Note that option type support is still incomplete.
*/

/** @group stdlib.optiontypes Return value of \a x if \a x is not absent. Aborts 
  when evaluated on absent value. */
function $T: deopt(opt $T: x);

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

predicate bool_opt_eq(var opt bool: x, var opt bool: y) =
  deopt(x)=deopt(y) /\ occurs(x)=occurs(y);

/** @group stdlib.optiontypes.bool True iff both \a b0 and \a b1 are absent or
  both are present and have the same value. */
predicate bool_eq(var opt bool: b0, var opt bool: b1) =
     (absent(b0) /\ absent(b1))
  \/ (occurs(b0) /\ occurs(b1) /\ deopt(b0)=deopt(b1));

/** @group stdlib.optiontypes.bool True iff \a b0 occurs and is equal to \a b1 */
predicate bool_eq(var opt bool: b0, var bool: b1) =
  occurs(b0) /\ deopt(b0)=b1;

  /** @group stdlib.optiontypes.bool True iff \a b1 occurs and is equal to \a b0 */
predicate bool_eq(var bool: b0, var opt bool: b1) =
  occurs(b1) /\ deopt(b1)=b0;

/** @group stdlib.optiontypes.bool Return truth value of \a x &and; \a y with identity <> = True*/
function var bool: '/\'(var opt bool: x, var opt bool: y) = (absent(x) \/ deopt(x)) /\ (absent(y) \/ deopt(y));
function bool: '/\'(opt bool: x, opt bool: y) = (absent(x) \/ deopt(x)) /\ (absent(y) \/ deopt(y));

/** @group stdlib.optiontypes.bool Return truth value of \a x &or; \a y with identity <> = False*/
function var bool: '\/'(var opt bool: x, var opt bool: y) = (occurs(x) /\ deopt(x)) \/ (occurs(y) /\ deopt(y));
function bool: '\/'(opt bool: x, opt bool: y) = (occurs(x) /\ deopt(x)) \/ (occurs(y) /\ deopt(y));

/** @group stdlib.optiontypes.bool True iff for any \p i, \a x[i] is absent or true */
predicate forall (array[int] of var opt bool: x) =
  forall ([absent(x[i]) \/ deopt(x[i]) | i in index_set(x)]);

/** @group stdlib.optiontypes.bool True iff for at least one \p i, \a x[i] occurs and is true */
predicate exists (array[int] of var opt bool: x) =
  exists ([occurs(x[i]) /\ deopt(x[i]) | i in index_set(x)]);

/** @group stdlib.optiontypes.bool Negation of \a x if it occurs, otherwise absent */
function var opt bool: 'not'(var opt bool: x) = if absent(x) then <> else not deopt(x) endif;
function opt bool: 'not'(opt bool: x) = if absent(x) then <> else not deopt(x) endif;

/** @group stdlib.optiontypes.bool Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt bool: element(var opt int: idx, array[int] of var bool: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group stdlib.optiontypes.bool Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt bool: element(var opt int: idx1, var opt int: idx2, array[int,int] of var bool: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group stdlib.optiontypes.bool Return \a x[\a idx] */
function var opt bool: element(var int: idx, array[int] of var opt bool: x) =
  let {
    var opt bool: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

/** @group stdlib.optiontypes.bool Return \a x[\a idx1, \a idx2] */
function var opt bool: element(var int: idx1, var int: idx2, array[int,int] of var opt bool: x) =
  let {
    var opt bool: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

/** @group stdlib.optiontypes.bool Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt bool: element(var opt int: idx, array[int] of var opt bool: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group stdlib.optiontypes.bool Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt bool: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt bool: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group stdlib.optiontypes.bool Search annotation for optional Boolean variables */
annotation bool_search(array[int] of var opt bool: x, ann: a1, ann: a2, ann: a3) =
  bool_search([if occurs(x[i]) then deopt(x[i]) else false endif | i in index_set(x)],a1,a2,a3);

/** @group stdlib.optiontypes.bool Search annotation for optional Boolean variables */
annotation bool_search(array[int] of var opt bool: x, ann: a1, ann: a2) =
  bool_search([if occurs(x[i]) then deopt(x[i]) else false endif | i in index_set(x)],a1,a2);


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

predicate var_dom(var opt int:x, set of int: s) =
  let {
    var int: dx = deopt(x);
    set of int: new_dom = dom(dx) intersect s;
  } in if new_dom = {} then absent(x) else dx in new_dom endif;

predicate var_dom(array[$T] of var opt int: x, set of int: d) =
    let { array[int] of var opt int: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));

predicate int_opt_eq(var opt int: x, var opt int: y) =
  deopt(x) = deopt(y) /\ occurs(x) = occurs(y);

/** @group stdlib.optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function var bool: '>'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function var bool: '>='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function var bool: '<'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function var bool: '<='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group stdlib.optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function bool: '>'(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function bool: '>='(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function bool: '<'(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function bool: '<='(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group stdlib.optiontypes.int Return minimum of elements in \a x that are not absent, or
absent if all elements in \a x are absent. */
function var opt int: min(array[int] of var opt int: x) ::promise_total =
    let {
        var opt lb_array(x)..ub_array(x): m;
        var lb_array(x)..ub_array(x): xmax;
        constraint if ub(xmax) != infinity then xmax = ub(xmax) else forall (i in index_set(x)) (xmax >= deopt(x[i])) endif;
        constraint occurs(m) <-> exists (i in index_set(x)) (occurs(x[i]));
        constraint occurs(m) ->
            deopt(m) = min([if occurs(xi) then deopt(xi) else xmax endif | xi in x]);
    } in m;

/** @group stdlib.optiontypes.int Return maximum of elements in \a x that are not absent, or
absent if all elements in \a x are absent. */
function var opt int: max(array[int] of var opt int: x) ::promise_total =
    let {
        var opt lb_array(x)..ub_array(x): m;
        var lb_array(x)..ub_array(x): xmin;
        constraint if lb(xmin) != -infinity then xmin = lb(xmin) else forall (i in index_set(x)) (xmin <= deopt(x[i])) endif;
        constraint occurs(m) <-> exists (i in index_set(x)) (occurs(x[i]));
        constraint occurs(m) ->
            deopt(m) = max([if occurs(xi) then deopt(xi) else xmin endif | xi in x]);
    } in m;

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

/** @group stdlib.optiontypes.int Return optional 0/1 integer that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt int: bool2int(var opt bool: x) ::promise_total =
  let {
    var opt 0..1: xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=bool2int(deopt(x));
  } in xi;

/** @group stdlib.optiontypes.int True iff both \a x and \a y are absent or
  both are present and have the same value. */
predicate int_eq(var opt int: x, var opt int: y) =
     (absent(x) /\ absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)=deopt(y))::maybe_partial);

/** @group stdlib.optiontypes.int True iff only one of \a x and \a y is absent or
  both are present and have different values. */
predicate int_ne(var opt int : x, var opt int : y) =
     (absent(x) != absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)!=deopt(y))::maybe_partial);

/** @group stdlib.optiontypes.int Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function var int: '+'(var opt int: x, var opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group stdlib.optiontypes.int Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function int: '+'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group stdlib.optiontypes.int Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function var opt int: '-'(var opt int: x, var opt int: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;

function var int: '-'(var int: x, var opt int: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;

/** @group stdlib.optiontypes.int Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function opt int: '-'(opt int: x, opt int: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;

function int: '-'(int: x, opt int: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;

/** @group stdlib.optiontypes.int Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function var int: '*'(var opt int: x, var opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;

/** @group stdlib.optiontypes.int Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function int: '*'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;

/** @group stdlib.optiontypes.int Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function var opt int: 'div'(var opt int: x, var opt int: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) div deopt(y) endif;

function var int: 'div'(var int: x, var opt int: y) =
  if absent(y) then x else x div deopt(y) endif;

/** @group stdlib.optiontypes.int Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function opt int: 'div'(opt int: x, opt int: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) div deopt(y) endif;

function int: 'div'(int: x, opt int: y) =
  if absent(y) then x else x div deopt(y) endif;

/** @group stdlib.optiontypes.int Optional modulo. Return absent if \a x or \a y is absent,
  \a x modulo \a y if both are present. */
function var opt int: 'mod'(var opt int: x, var opt int: y) =
  if occurs(x) /\ occurs(y) then deopt(x) mod deopt(y) else <> endif;

/** @group stdlib.optiontypes.int Optional modulo. Return absent if \a x or \a y is absent,
  \a x modulo \a y if both are present. */
function opt int: 'mod'(opt int: x, opt int: y) =
  if occurs(x) /\ occurs(y) then deopt(x) mod deopt(y) else <> endif;

/** @group stdlib.optiontypes.int Return sum of non-absent elements of \a x. */
function var int: sum(array[int] of var opt int: x) =
  sum (i in index_set(x)) (let { var int: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 0 endif);

/** @group stdlib.optiontypes.int Return sum of non-absent elements of \a x. */
function int: sum(array[int] of opt int: x) =
  sum (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 0 endif);

/** @group stdlib.optiontypes.int Return product of non-absent elements of \a x. */
function var int: product(array[int] of var opt int: x) =
  product (i in index_set(x)) (let { var int: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 1 endif);

/** @group stdlib.optiontypes.int Return product of non-absent elements of \a x. */
function int: product(array[int] of opt int: x) =
  product (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 1 endif);

/** @group stdlib.optiontypes.int Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt int: element(var opt int: idx, array[int] of var int: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group stdlib.optiontypes.int Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt int: element(var opt int: idx1, var opt int: idx2, array[int,int] of var int: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group stdlib.optiontypes.int Return \a x[\a idx] */
function var opt int: element(var int: idx, array[int] of var opt int: x) =
  let {
    var opt int: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

/** @group stdlib.optiontypes.int Return \a x[\a idx1, \a idx2] */
function var opt int: element(var int: idx1, var int: idx2, array[int,int] of var opt int: x) =
  let {
    var opt int: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

/** @group stdlib.optiontypes.int Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt int: element(var opt int: idx, array[int] of var opt int: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group stdlib.optiontypes.int Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt int: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt int: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group stdlib.optiontypes.int Search annotation for optional integer variables */
annotation int_search(array[int] of var opt int: x, ann: a1, ann: a2, ann: a3) =
  int_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2,a3);

/** @group stdlib.optiontypes.int Search annotation for optional integer variables */
annotation int_search(array[int] of var opt int: x, ann: a1, ann: a2) =
  int_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2);

/* Internal function used to optimize over option type objective */

function var int: objective_deopt_(var opt int: x, bool: direction) =
  let {
    int: worst = if direction then lb(x)-1 else ub(x)+1 endif;
  } in if occurs(x) then deopt(x) else worst endif;

/** @group flatzinc.set Constrains \a x \( \in \) \a S */
predicate set_in(var opt int: x, set of int: S) = if occurs(x) then deopt(x) in S endif;
/** @group flatzinc.set Constrains \a x \( \in \) \a S */
predicate set_in(var opt int: x, var set of int: S) = if occurs(x) then deopt(x) in S endif;

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

/** @group stdlib.optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function var bool: '>'(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function var bool: '>='(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function var bool: '<'(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function var bool: '<='(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group stdlib.optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function bool: '>'(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function bool: '>='(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function bool: '<'(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function bool: '<='(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group stdlib.optiontypes.float Return minimum of elements in \a x that are not absent, or
absent if all elements in \a x are absent. */
% :NOTE: since -oo is always a lb for float variables, should we instead return -oo if all absent?
function var opt float: min(array[int] of var opt float: x) ::promise_total =
    let {
        var opt lb_array(x)..ub_array(x): m;
        var lb_array(x)..ub_array(x): xmax;
        constraint if ub(xmax) != infinity then xmax = ub(xmax) else forall (i in index_set(x)) (xmax >= deopt(x[i])) endif;
        constraint occurs(m) <-> exists (i in index_set(x)) (occurs(x[i]));
        constraint occurs(m) ->
            deopt(m) = min([if occurs(xi) then deopt(xi) else xmax endif | xi in x]);
    } in m;

/** @group stdlib.optiontypes.float Return maximum of elements in \a x that are not absent, or
absent if all elements in \a x are absent. */
function var opt float: max(array[int] of var opt float: x) ::promise_total =
    let {
        var opt lb_array(x)..ub_array(x): m;
        var lb_array(x)..ub_array(x): xmin;
        constraint if lb(xmin) != -infinity then xmin = lb(xmin) else forall (i in index_set(x)) (xmin <= deopt(x[i])) endif;
        constraint occurs(m) <-> exists (i in index_set(x)) (occurs(x[i]));
        constraint occurs(m) ->
            deopt(m) = max([if occurs(xi) then deopt(xi) else xmin endif | xi in x]);
    } in m;

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

/** @group stdlib.optiontypes.float Return optional 0/1 float that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt float: bool2float(var opt bool: x) ::promise_total =
  let {
    var opt 0.0..1.0: xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=bool2float(deopt(x));
  } in xi;

/** @group stdlib.optiontypes.int Return optional 0/1 integer that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt float: int2float(var opt int: x) ::promise_total =
  let {
    var opt int2float(lb(x))..int2float(ub(x)): xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=int2float(deopt(x));
  } in xi;


/** @group stdlib.optiontypes.float True iff both \a x and \a y are absent or
  both are present and have the same value. */
predicate float_eq(var opt float: x, var opt float: y) =
     (absent(x) /\ absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)=deopt(y))::maybe_partial);

/** @group stdlib.optiontypes.float True iff only one of \a x and \a y is absent or
  both are present and have different values. */
predicate float_ne(var opt float : x, var opt float : y) =
     (absent(x) != absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)!=deopt(y))::maybe_partial);

/** @group stdlib.optiontypes.float Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function var float: '+'(var opt float: x, var opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group stdlib.optiontypes.float Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function float: '+'(opt float: x, opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group stdlib.optiontypes.float Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function var opt float: '-'(var opt float: x, var opt float: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;

function var float: '-'(var float: x, var opt float: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;

/** @group stdlib.optiontypes.float Optional subtraction. Return absent if \a x is absent, \a x if \a y is absent,
  difference of \a x and \a y if both are present. */
function opt float: '-'(opt float: x, opt float: y) ::promise_total =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x)-deopt(y) endif;

function float: '-'(float: x, opt float: y) ::promise_total =
  if absent(y) then x else x-deopt(y) endif;

/** @group stdlib.optiontypes.float Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function var float: '*'(var opt float: x, var opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;

/** @group stdlib.optiontypes.float Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function float: '*'(opt float: x, opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;

/** @group stdlib.optiontypes.float Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function var opt float: '/'(var opt float: x, var opt float: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) / deopt(y) endif;

function var float: '/'(var float: x, var opt float: y) =
  if absent(y) then x else x / deopt(y) endif;

/** @group stdlib.optiontypes.float Optional division. Return absent if \a x is absent, \a x if \a y is absent,
  \a x divided by \a y if both are present. */
function opt float: '/'(opt float: x, opt float: y) =
  if absent(x) then <> elseif absent(y) then deopt(x) else deopt(x) / deopt(y) endif;

function float: '/'(float: x, opt float: y) =
  if absent(y) then x else x / deopt(y) endif;

/** @group stdlib.optiontypes.float Return sum of non-absent elements of \a x. */
function var float: sum(array[int] of var opt float: x) =
  sum (i in index_set(x)) (let { var float: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 0 endif);

/** @group stdlib.optiontypes.float Return sum of non-absent elements of \a x. */
function float: sum(array[int] of opt float: x) =
  sum (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 0 endif);

/** @group stdlib.optiontypes.float Return product of non-absent elements of \a x. */
function var float: product(array[int] of var opt float: x) =
  product (i in index_set(x)) (let { var float: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 1 endif);

/** @group stdlib.optiontypes.float Return product of non-absent elements of \a x. */
function float: product(array[int] of opt float: x) =
  product (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 1 endif);

/** @group stdlib.optiontypes.float Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt float: element(var opt int: idx, array[int] of var float: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group stdlib.optiontypes.float Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt float: element(var opt int: idx1, var opt int: idx2, array[int,int] of var float: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group stdlib.optiontypes.float Return \a x[\a idx] */
function var opt float: element(var int: idx, array[int] of var opt float: x) =
  let {
    var opt float: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

/** @group stdlib.optiontypes.float Return \a x[\a idx1, \a idx2] */
function var opt float: element(var int: idx1, var int: idx2, array[int,int] of var opt float: x) =
  let {
    var opt float: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

/** @group stdlib.optiontypes.float Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt float: element(var opt int: idx, array[int] of var opt float: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group stdlib.optiontypes.float Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt float: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt float: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group stdlib.optiontypes.float Search annotation for optional float variables */
annotation float_search(array[int] of var opt float: x, ann: a1, ann: a2, ann: a3) =
  float_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2,a3);

/** @group stdlib.optiontypes.float Search annotation for optional float variables */
annotation float_search(array[int] of var opt float: x, ann: a1, ann: a2) =
  float_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2);

/* Internal function used to optimize over option type objective */

function var float: objective_deopt_(var opt float: x, bool: direction) =
  let {
    float: worst = if direction then lb(x)-1 else ub(x)+1 endif;
  } in if occurs(x) then deopt(x) else worst endif;

