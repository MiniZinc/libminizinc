include "sort.mzn";

/** @group globals.sort
  Return a multiset of values that is the same as the
  multiset of values in \a x but in sorted order.
*/
function array[int] of var $$E: sort(array[int] of var $$E: x) ::promise_total =
    let {
      array[1..length(x)] of var lb_array(x)..ub_array(x): y ::is_defined_var;
      constraint sort(x,y) ::defines_var(y);
    } in y;
