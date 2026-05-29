include "nvalue.mzn";

/** @group globals.alldifferent
  Returns the number of distinct values in \a xs.
*/
function var int: nvalue(array [$X] of var $$E: xs) :: promise_total =
  let {
    var bool2int(length(xs) > 0)..min(length(xs), card(dom_array(xs))): n :: is_defined_var;
    constraint nvalue(n, xs) :: defines_var(n);
  } in n;

/** @group globals.alldifferent
  Returns the number of distinct values in \a xs.
*/
function var int: nvalue(
  array [$X] of var opt $$E: xs,
) =
  let {
    any: xdom = enum2int(dom_array(xs));
    int: def = if 0 in xdom then min(xdom) - 1 else 0 endif;
  } in nvalue([x_i default def | x_i in enum2int(xs)] ++ [def]) - 1;
