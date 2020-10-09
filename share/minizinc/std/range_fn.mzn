include "range.mzn";

/** @group globals
  Returns the image of function \a x (represented as an array) on set
  of values \a s.  ub(\a s) must be a subset of index_set(\a x) otherwise an
  assertion failure will occur.
*/
function var set of int: range(array[int] of var int: x, var set of int: s) 
    ::promise_total =
    let {
      var set of lb_array(x)..ub_array(x): t ::is_defined_var;
      constraint range(x,s,t) ::defines_var(t);
    } in t;
