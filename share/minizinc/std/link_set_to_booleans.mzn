include "fzn_link_set_to_booleans.mzn";
include "fzn_link_set_to_booleans_reif.mzn";

/** @group globals.channeling
  Constrains the array of Booleans \a b to be a representation of the set \a s:
  \p i in \a s \( \leftrightarrow \) \a b[\p i].
  
  The index set of \a b must be a superset of the possible values of \a s.
*/
predicate link_set_to_booleans(var set of $$E: s, array [$$E] of var bool: b) =
  assert(
    ub(s) subset index_set(b),
    "link_set_to_booleans: the index set of b must be a superset of the possible values of s",
    fzn_link_set_to_booleans(enum2int(s), index2int(b)),
  );
