include "fzn_table_bool.mzn";
include "fzn_table_bool_reif.mzn";
include "fzn_table_int.mzn";
include "fzn_table_int_reif.mzn";
include "fzn_table_int_opt.mzn";

predicate fzn_table_bool(array [int] of var bool: xs, array [int, int] of bool: tuples);
predicate fzn_table_int(array [int] of var int: xs, array [int, int] of int: tuples);
predicate fzn_table_int_opt(array [int] of var opt int: xs, array [int, int] of opt int: tuples);

/** @group globals.extensional
  Represents the constraint \a xs in \a tuples where we
  consider each row in \a tuples to be a tuple and \a tuples as a set of tuples.
*/
predicate table(array [$$E] of var bool: xs, array [$$X, $$E] of bool: tuples) =
  assert(
    index_set_2of2(tuples) == index_set(xs),
    "The second dimension of the table must equal the number of variables " ++
      "in the first argument",
  ) /\
    fzn_table_bool(index2int(xs), index2int(tuples));

/** @group globals.extensional
  Represents the constraint \a xs in \a tuples where we
  consider each row in \a tuples to be a tuple and \a tuples as a set of tuples.
*/
predicate table(array [$$E] of var int: xs, array [$$X, $$E] of int: tuples) =
  assert(
    index_set_2of2(tuples) == index_set(xs),
    "The second dimension of the table must equal the number of variables " ++
      "in the first argument",
  ) /\
    fzn_table_int(index2int(xs), index2int(tuples));

/** @group globals.extensional
  Represents the constraint \a xs in \a tuples if the variable \a x_i and the value
  \a t_i occur where we consider each row in \a tuples to be a tuple and \a tuples as a
  set of tuples.
*/
predicate table(array [$$E] of var opt int: xs, array [$$X, $$E] of opt int: tuples) =
  assert(
    index_set_2of2(tuples) == index_set(xs),
    "The second dimension of the table must equal the number of variables " ++
      "in the first argument",
  ) /\
    fzn_table_int_opt(index2int(xs), index2int(tuples));
