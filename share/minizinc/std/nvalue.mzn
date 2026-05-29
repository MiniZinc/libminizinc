include "fzn_nvalue.mzn";
include "fzn_nvalue_reif.mzn";

/** @group globals.alldifferent
  Requires that the number of distinct values in \a xs is \a n.
*/
predicate nvalue(var int: n, array [$X] of var $$E: xs) =
  if length(xs) = 0 then n=0 else fzn_nvalue(n, array1d(enum2int(xs))) endif;

/** @group globals.alldifferent
  Requires that the number of distinct values in \a xs is \a n.
*/
predicate nvalue(var int: n, array [$X] of var opt $$E: xs) =
  if length(xs) = 0 then n=0 else
  let {
    any: xdom = enum2int(dom_array(xs));
    int: def = if 0 in xdom then min(xdom) - 1 else 0 endif;
  } in nvalue(n + 1, [x_i default def | x_i in enum2int(xs)] ++ [def])
  endif;
