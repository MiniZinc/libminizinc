include "fzn_disjoint.mzn";
include "fzn_disjoint_reif.mzn";

/** @group globals.set Requires that sets \a s1 and \a s2 do not intersect. */
predicate disjoint(var set of $$E: s1, var set of $$E: s2) =
  fzn_disjoint(enum2int(s1), enum2int(s2));
