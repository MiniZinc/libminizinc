include "fzn_range.mzn";
include "fzn_range_reif.mzn";

/** @group globals.math
  Requires that the image of function \a xs (represented as an array) on set
  of values \a domain is \a image.  ub(\a domain) must be a subset of index_set(\a xs) otherwise an
  assertion failure will occur.
*/
predicate range(array [$$X] of var $$Y: xs, var set of $$X: domain, var set of $$Y: image) =
  assert(
    ub(domain) subset index_set(xs),
    "range: upper bound of 'domain' must be a subset of the index set of 'xs'",
    fzn_range(index2int(enum2int(xs)), enum2int(domain), enum2int(image)),
  );
