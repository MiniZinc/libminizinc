/** @group globals.alldifferent
  Requires that the number of distinct values in \a x is \a n.
*/
predicate nvalue(var int: n, array[int] of var int: x) =
    let { int: lx = lb_array(x),
          int: ux = ub_array(x),
        } in
        n == sum(j in lx..ux) (
	   bool2int(exists(i in index_set(x)) ( x[i] = j ))
        );

/** @group globals.alldifferent
  Returns the number of distinct values in \a x.
*/
function var int: nvalue(array[int] of var int: x) =
  let { var 0..length(x): n; constraint nvalue(n,x); } in n;
