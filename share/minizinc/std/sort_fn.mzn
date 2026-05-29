include "sort.mzn";

/** @group globals.sort
  Return a multiset of values that is the same as the
  multiset of values in \a xs but in sorted order.
*/
function array [$$E] of var $$T: sort(array [$$E] of var $$T: xs) :: promise_total =
  let {
    array [index_set(xs)] of var lb_array(xs)..ub_array(xs): y :: is_defined_var;
    constraint sort(xs, y) :: defines_var(y);
  } in y;
