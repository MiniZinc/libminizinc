include "among.mzn";

/**  @group globals.counting
  Returns the number of variables in \a x that take one of the values in \a v.
*/
function var int: among(array[int] of var int:x, set of int:v) :: promise_total =
    let { var 0..length(x): n ::is_defined_var; 
          constraint among(n,x,v) ::defines_var(n); }
    in n;
