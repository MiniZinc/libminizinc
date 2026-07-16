include "global_cardinality_closed.mzn";

/** @group globals.counting
  Returns an array with number of occurrences of \a cover[\p i] in \a xs.

  The elements of \a xs must take their values from \a cover.
*/
function array [$Y] of var int: global_cardinality_closed(
  array [$X] of var $$E: xs,
  array [$Y] of $$E: cover,
)
  :: promise_total =
  let {
    any: cover1d = array1d(cover);
    array [index_set(cover1d)] of var 0..length(xs): count :: is_defined_var;
    constraint global_cardinality_closed(array1d(xs), cover1d, count) :: defines_var(count);
  } in arrayXd(cover, count);

/** @group globals.counting
  Returns an array with number of occurrences of \a cover[\p i] in \a xs.

  The elements of \a xs must take their values from \a cover or be absent.
*/
function array [$Y] of var int: global_cardinality_closed(
  array [$X] of var opt $$E: xs,
  array [$Y] of $$E: cover,
)
  :: promise_total =
  let {
    any: cover1d = array1d(cover);
    array [index_set(cover1d)] of var 0..length(xs): count :: is_defined_var;
    constraint global_cardinality_closed(array1d(xs), cover1d, count) :: defines_var(count);
  } in arrayXd(cover, count);
