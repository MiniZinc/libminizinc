include "fzn_among.mzn";

/**  @group globals.counting
  Returns the number of variables in \a xs that take one of the values in \a values.
*/
function var int: among(array [$X] of var $$E: xs, set of $$E: values) :: promise_total =
  let {
    var 0..length(xs): n :: is_defined_var;
    constraint fzn_among(n, enum2int(array1d(xs)), enum2int(values)) :: defines_var(n);
  } in n;

/**  @group globals.counting
  Returns the number of variables in \a xs that take one of the values in \a values.
*/
function int: among(array [$X] of $$E: xs, set of $$E: values) = count (xi in xs) (xi in values);
