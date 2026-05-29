include "fzn_value_precede_int.mzn";
include "fzn_value_precede_int_reif.mzn";
include "fzn_value_precede_int_opt.mzn";
include "fzn_value_precede_set.mzn";
include "fzn_value_precede_set_reif.mzn";

/** @group globals.lexicographic
  Requires that \a before precede \a after in the array \a xs.

  Precedence means that if any element of \a xs
  is equal to \a after, then another element of \a xs with a lower index is equal
  to \a before.
*/
predicate value_precede($$E: before, $$E: after, array [$$X] of var $$E: xs) =
  fzn_value_precede_int(enum2int(before), enum2int(after), index2int(enum2int(xs)));

/** @group globals.lexicographic
  Requires that \a before precede \a after in the array \a xs.

  Precedence means that if any element of \a xs
  is equal to \a after, then another element of \a xs with a lower index is equal
  to \a before.
*/
predicate value_precede($$E: before, $$E: after, array [$$X] of var opt $$E: xs) =
  fzn_value_precede_int_opt(enum2int(before), enum2int(after), index2int(enum2int(xs)));

/** @group globals.lexicographic
  Requires that \a before precede \a after in the array \a xs.

  Precedence means that if an element of \a xs
  contains \a after but not \a before, then another element of \a xs with lower index contains
  \a before but not \a after.
*/
predicate value_precede($$E: before, $$E: after, array [$$X] of var set of $$E: xs) =
  fzn_value_precede_set(enum2int(before), enum2int(after), index2int(enum2int(xs)));
