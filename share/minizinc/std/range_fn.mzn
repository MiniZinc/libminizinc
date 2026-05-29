include "range.mzn";

/** @group globals.math
  Returns the image of function \a xs (represented as an array) on set
  of values \a domain.  ub(\a domain) must be a subset of index_set(\a xs) otherwise an
  assertion failure will occur.
*/
function var set of int: range(array [int] of var int: xs, var set of int: domain) :: promise_total =
  let {
    var set of lb_array(xs)..ub_array(xs): image :: is_defined_var;
    constraint range(xs, domain, image) :: defines_var(image);
  } in image;
