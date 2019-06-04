/**  @group globals.counting
  Requires exactly \a n variables in \a x to take one of the values in \a v.
*/
predicate fzn_among(var int: n, array[int] of var int: x, set of int: v) =
    n == sum(i in index_set(x)) ( bool2int(x[i] in v) );

