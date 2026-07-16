include "distribute.mzn";

/** @group globals.counting
  Returns an array of the number of occurrences of \a cover[\p i] in
  \a xs. The values in \a cover need not be distinct.
*/
function array [$X] of var int: distribute(
  array [$X] of var $$E: cover,
  array [$Y] of var $$E: xs,
)
  :: promise_total =
  let {
    array [int] of var int: value1d = array1d(enum2int(cover));
    array [index_set(value1d)] of var 0..length(xs): count :: is_defined_var;
    constraint distribute(count, value1d, array1d(enum2int(xs))) :: defines_var(count);
  } in arrayXd(cover, count);
