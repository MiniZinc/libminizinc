include "global_cardinality.mzn";

/** @group globals.counting
  Returns the number of occurrences of \a cover[\p i] in \a x.
*/
function array[$Y] of var int: global_cardinality(array[$X] of var $$E: x,
                                                  array[$Y] of $$E: cover) :: promise_total =
  let { array[int] of int: cover1d = array1d(cover);
        array[index_set(cover1d)] of var 0..length(x): counts ::is_defined_var;
        constraint global_cardinality(array1d(x), cover1d, counts) ::defines_var(counts); } in
  arrayXd(cover, counts);

/** @group globals.counting
  Returns the number of occurrences of \a cover[\p i] in \a x.
*/
function array[$Y] of var int: global_cardinality(array[$X] of var opt $$E: x,
                                                  array[$Y] of $$E: cover) :: promise_total = 
  let { array[int] of int: cover1d = array1d(cover);
        array[index_set(cover1d)] of var 0..length(x): counts ::is_defined_var;
        constraint global_cardinality(array1d(x), cover1d, counts) ::defines_var(counts); } in
  arrayXd(cover, counts);
