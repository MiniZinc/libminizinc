/***
  @groupdef stdlib.reflect Reflection operations

  These functions return information about declared or inferred variable
  bounds and domains.
*/

/** @group stdlib.reflect Return lower bound of \a x */
function int: lb(var int: x);
/** @group stdlib.reflect Return upper bound of \a x */
function int: ub(var int: x);
/** @group stdlib.reflect Return lower bound of \a x */
function int: lb(var opt int: x);
/** @group stdlib.reflect Return upper bound of \a x */
function int: ub(var opt int: x);
/** @group stdlib.reflect Return lower bound of \a x */
function float: lb(var float: x);
/** @group stdlib.reflect Return upper bound of \a x */
function float: ub(var float: x);
/** @group stdlib.reflect Return lower bound of \a x */
function float: lb(var opt float: x);
/** @group stdlib.reflect Return upper bound of \a x */
function float: ub(var opt float: x);
/** @group stdlib.reflect Return lower bound of \a x */
function set of int: lb(var set of int: x);
/** @group stdlib.reflect Return upper bound of \a x */
function set of int: ub(var set of int: x);
/** @group stdlib.reflect Return array of lower bounds of the elements in array \a x */
function array[$U] of int: lb(array[$U] of var int: x) =
  arrayXd(x,[lb(xx) | xx in array1d(x)]);
/** @group stdlib.reflect Return array of upper bounds of the elements in array \a x */
function array[$U] of int: ub(array[$U] of var int: x) =
  arrayXd(x,[ub(xx) | xx in array1d(x)]);
/** @group stdlib.reflect Return array of lower bounds of the elements in array \a x */
function array[$U] of float: lb(array[$U] of var float: x) =
  arrayXd(x,[lb(xx) | xx in array1d(x)]);
/** @group stdlib.reflect Return array of upper bounds of the elements in array \a x */
function array[$U] of float: ub(array[$U] of var float: x) =
  arrayXd(x,[ub(xx) | xx in array1d(x)]);
/** @group stdlib.reflect Return array of lower bounds of the elements in array \a x */
function array[$U] of set of int: lb(array[$U] of var set of int: x) =
  arrayXd(x,[lb(xx) | xx in array1d(x)]);
/** @group stdlib.reflect Return array of upper bounds of the elements in array \a x */
function array[$U] of set of int: ub(array[$U] of var set of int: x) =
  arrayXd(x,[ub(xx) | xx in array1d(x)]);
/** @group stdlib.reflect Return minimum of all lower bounds of the elements in array \a x */
function int: lb_array(array[$U] of var opt int: x);
/** @group stdlib.reflect Return maximum of all upper bounds of the elements in array \a x */
function int: ub_array(array[$U] of var opt int: x);
/** @group stdlib.reflect Return minimum of all lower bounds of the elements in array \a x */
function float: lb_array(array[$U] of var opt float: x);
/** @group stdlib.reflect Return maximum of all upper bounds of the elements in array \a x */
function float: ub_array(array[$U] of var opt float: x);
/** @group stdlib.reflect Return intersection of all lower bounds of the elements in array \a x */
function set of int: lb_array(array[$U] of var set of int: x);
/** @group stdlib.reflect Return union of all upper bounds of the elements in array \a x */
function set of int: ub_array(array[$U] of var set of int: x);
/** @group stdlib.reflect Return domain of \a x */
function set of int: dom(var int: x);
/** @group stdlib.reflect Return domain of \a x */
function set of int: dom(var opt int: x);
function set of int: dom(var bool: b) =
  if is_fixed(b) then if fix(b) then {1} else {0} endif else {0,1} endif;

/** @group stdlib.reflect Return union of all domains of the elements in array \a x */
function set of int: dom_array(array[$T] of var int: x);
/** @group stdlib.reflect Return union of all domains of the elements in array \a x */
function set of int: dom_array(array[$T] of var opt int: x);
/** @group stdlib.reflect Return approximation of union of all domains of the elements in array \a x */
function set of int: dom_bounds_array(array[$T] of var int: x);
/** @group stdlib.reflect Return approximation of union of all domains of the elements in array \a x */
function set of int: dom_bounds_array(array[$T] of var opt int: x);
/** @group stdlib.reflect Return cardinality of the domain of \a x */
function int: dom_size(var int: x) = card(dom(x));

/** @group stdlib.reflect Test if variable \a x has declared, finite bounds */
function par bool: has_bounds(var int: x);
/** @group stdlib.reflect Test if variable \a x has declared, finite bounds */
function par bool: has_bounds(var float: x);
/** @group stdlib.reflect Test if variable \a x has a declared, finite upper bound */
function par bool: has_ub_set(var set of int: x);

/** @group stdlib.reflect Check if the value of \a x is fixed at this point
  in evaluation. If it is fixed, return its value, otherwise abort. */
function $T: fix(var opt $T: x);
/** @group stdlib.reflect Check if the value of every element of the array \a x is fixed
at this point in evaluation. If all are fixed, return an array of their values, otherwise abort. */
function array[$U] of $T: fix(array[$U] of var opt $T: x);
/** @group stdlib.reflect Test if \a x is fixed */
function bool: is_fixed(var opt $T: x);
/** @group stdlib.reflect Test if \a x is fixed */
function bool: is_fixed(set of $T: x);
/** @group stdlib.reflect Test if \a x is fixed */
function bool: is_fixed(var set of int: x);
/** @group stdlib.reflect Test if every element of array \a x is fixed */
function bool: is_fixed(array[$U] of var opt $T: x);

/** @group stdlib.reflect Test if \a x is annotated \a a */
function bool: has_ann(var opt $T: x, ann: a);
/** @group stdlib.reflect Test if \a x is annotated \a a */
function bool: has_ann(var set of $T: x, ann: a);
/** @group stdlib.reflect Test if \a x is annotated \a a */
function bool: has_ann(var set of int: x, ann: a);

/** @group stdlib.reflect Annotate declaration of \a x with annotation \a a */
function bool: annotate(var opt $T: x, ann: a);
/** @group stdlib.reflect Annotate declaration of \a x with annotation \a a */
function bool: annotate(set of $T: x, ann: a);
/** @group stdlib.reflect Annotate declaration of \a x with annotation \a a */
function bool: annotate(var set of int: x, ann: a);

/** @group stdlib.reflect Test if \a x and \a y are the same variable */
function bool: is_same(var opt $T: x, var opt $U: y);

