include "fzn_disjoint.mzn";
include "fzn_disjoint_reif.mzn";

/** @group globals.set Requires that sets \a s and \a t do not intersect. */
predicate disjoint(var set of $$E: s, var set of $$E: t) =
  fzn_disjoint(enum2int(s), enum2int(t));
