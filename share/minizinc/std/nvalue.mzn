include "fzn_nvalue.mzn";
include "fzn_nvalue_reif.mzn";

/** @group globals.alldifferent
  Requires that the number of distinct values in \a x is \a n.
*/
predicate nvalue(var int: n, array[$X] of var int: x) =
	fzn_nvalue(n, array1d(x));

/** @group globals.alldifferent
  Requires that the number of distinct values in \a x is \a n.
*/
predicate nvalue(var int: n, array[$X] of var opt int: x) =
  let {
    any: xdom = dom_array(x);
    int: def = if 0 in xdom then min(xdom) - 1 else 0 endif;
  } in nvalue(n + 1, [x_i default def | x_i in x] ++ [def]);
