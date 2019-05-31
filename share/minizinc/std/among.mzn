include "fzn_among.mzn";

/**  @group globals.counting
  Requires exactly \a n variables in \a x to take one of the values in \a v.
*/
predicate among(var int: n, array[int] of var int: x, set of int: v) =
    fzn_among(n, x, v);

/**  @group globals.counting
  Returns the number of variables in \a x that take one of the values in \a v.
*/
function var int: among(array[int] of var int:x, set of int:v) :: promise_total =
    let { var 0..length(x): n; 
          constraint fzn_among(n,x,v); }
    in n;
