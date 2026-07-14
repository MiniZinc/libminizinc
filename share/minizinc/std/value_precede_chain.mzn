include "fzn_value_precede_chain_int.mzn";
include "fzn_value_precede_chain_int_reif.mzn";
include "fzn_value_precede_chain_int_opt.mzn";
include "fzn_value_precede_chain_set.mzn";
include "fzn_value_precede_chain_set_reif.mzn";

predicate fzn_value_precede_chain_int(array [int] of int: order, array [int] of var int: xs);
predicate fzn_value_precede_chain_int_opt(
  array [int] of int: order,
  array [int] of var opt int: xs,
);
predicate fzn_value_precede_chain_set(
  array [int] of int: order,
  array [int] of var set of int: xs,
);

/** @group globals.lexicographic
  Requires that \a order[\p i] precedes \a order[\p i +1] in the array \a xs.
  
  Precedence means that if any element of \a xs
  is equal to \a order[\p i +1], then another element of \a xs with a  lower index is equal
  to \a order[\p i].
*/
predicate value_precede_chain(array [$$X] of $$E: order, array [$$Y] of var $$E: xs) =
  fzn_value_precede_chain_int(index2int(enum2int(order)), index2int(enum2int(xs)));

/** @group globals.lexicographic
  Requires that \a order[\p i] precedes \a order[\p i +1] in the array \a xs.
  
  Precedence means that if any element of \a xs
  is equal to \a order[\p i +1], then another element of \a xs with a  lower index is equal
  to \a order[\p i].
*/
predicate value_precede_chain(array [$$X] of $$E: order, array [$$Y] of var opt $$E: xs) =
  fzn_value_precede_chain_int_opt(index2int(enum2int(order)), index2int(enum2int(xs)));

/** @group globals.lexicographic
  Requires that \a order[\p i] precedes \a order[\p i +1] in the array \a xs.

  Precedence means that if an element of \a xs
  contains \a order[\p i +1] but not \a order[\p i], then another element of \a xs with lower index contains
  \a order[\p i] but not \a order[\p i +1].
*/
predicate value_precede_chain(array [$$X] of $$E: order, array [$$Y] of var set of $$E: xs) =
  fzn_value_precede_chain_set(index2int(enum2int(order)), index2int(enum2int(xs)));
