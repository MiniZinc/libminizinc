include "fzn_value_precede_int.mzn";
include "fzn_value_precede_int_reif.mzn";
include "fzn_value_precede_int_opt.mzn";
include "fzn_value_precede_set.mzn";
include "fzn_value_precede_set_reif.mzn";

/** @group globals.lexicographic
  Requires that \a s precede \a t in the array \a x.

  Precedence means that if any element of \a x
  is equal to \a t, then another element of \a x with a lower index is equal
  to \a s.
*/
predicate value_precede($$E: s, $$E: t, array [$$X] of var $$E: x) =
  fzn_value_precede_int(enum2int(s), enum2int(t), index2int(enum2int(x)));

/** @group globals.lexicographic
  Requires that \a s precede \a t in the array \a x.

  Precedence means that if any element of \a x
  is equal to \a t, then another element of \a x with a lower index is equal
  to \a s.
*/
predicate value_precede($$E: s, $$E: t, array [$$X] of var opt $$E: x) =
  fzn_value_precede_int_opt(enum2int(s), enum2int(t), index2int(enum2int(x)));

/** @group globals.lexicographic
  Requires that \a s precede \a t in the array \a x.

  Precedence means that if an element of \a x
  contains \a t but not \a s, then another element of \a x with lower index contains
  \a s but not \a t.
*/
predicate value_precede($$E: s, $$E: t, array [$$X] of var set of $$E: x) =
  fzn_value_precede_set(enum2int(s), enum2int(t), index2int(enum2int(x)));
