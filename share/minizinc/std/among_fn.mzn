include "fzn_among.mzn";

/**  @group globals.counting
  Returns the number of variables in \a x that take one of the values in \a v.
*/
function var int: among(array [$X] of var $$E: x, set of $$E: v) :: promise_total =
  let {
    var 0..length(x): n :: is_defined_var;
    constraint fzn_among(n, enum2int(array1d(x)), enum2int(v)) :: defines_var(n);
  } in n;

/**  @group globals.counting
  Returns the number of variables in \a xi that take one of the values in \a v.
*/
function int: among(array [$X] of $$E: xi, set of $$E: v) = count (x in xi) (x in v);
