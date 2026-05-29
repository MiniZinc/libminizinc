include "fzn_roots.mzn";
include "fzn_roots_reif.mzn";

/** @group globals.set
  Requires that \a xs[\p i] in \a image for all \p i in \a preimage, i.e.
  \a preimage is the set of indices \p i for which \a xs[\p i] is in \a image.
*/
predicate roots(array [$$X] of var $$Y: xs, var set of $$X: preimage, var set of $$Y: image) =
  assert(
    ub(preimage) subset index_set(xs),
    "roots: upper bound of 'preimage' must be a subset of the index set of 'xs'",
  ) /\
    fzn_roots(index2int(enum2int(xs)), enum2int(preimage), enum2int(image));
