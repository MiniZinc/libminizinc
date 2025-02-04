include "nvalue.mzn";

/** @group globals.alldifferent
  Returns the number of distinct values in \a x.
*/
function var int: nvalue(array [$X] of var $$E: x) :: promise_total =
  let {
    var bool2int(length(x) > 0)..min(length(x), card(dom_array(x))): n :: is_defined_var;
    constraint nvalue(n, x) :: defines_var(n);
  } in n;

/** @group globals.alldifferent
  Returns the number of distinct values in \a x.
*/
function var int: nvalue(
  array [$X] of var opt $$E: x,
) =
  let {
    any: xdom = enum2int(dom_array(x));
    int: def = if 0 in xdom then min(xdom) - 1 else 0 endif;
  } in nvalue([x_i default def | x_i in enum2int(x)] ++ [def]) - 1;
