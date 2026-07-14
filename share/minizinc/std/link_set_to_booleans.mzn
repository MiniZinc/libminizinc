include "fzn_link_set_to_booleans.mzn";
include "fzn_link_set_to_booleans_reif.mzn";

predicate fzn_link_set_to_booleans(var set of int: s, array [int] of var bool: bs);

/** @group globals.channeling
  Constrains the array of Booleans \a bs to be a representation of the set \a s:
  \p i in \a s \( \leftrightarrow \) \a bs[\p i].
  
  The index set of \a bs must be a superset of the possible values of \a s.
*/
predicate link_set_to_booleans(var set of $$E: s, array [$$E] of var bool: bs) =
  assert(
    ub(s) subset index_set(bs),
    "link_set_to_booleans: the index set of bs must be a superset of the possible values of s",
    fzn_link_set_to_booleans(enum2int(s), index2int(bs)),
  );
