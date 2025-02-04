include "fzn_sort.mzn";
include "fzn_sort_reif.mzn";

/** @group globals.sort
  Requires that the multiset of values in \a x are the same as the
  multiset of values in \a y but \a y is in sorted order.
*/
predicate sort(array [$$E] of var $$T: x, array [$$F] of var $$T: y) =
  assert(
    length(x) == length(y),
    "sort: x and y must be same sized arrays",
    fzn_sort(index2int(enum2int(x)), index2int(enum2int(y))),
  );
