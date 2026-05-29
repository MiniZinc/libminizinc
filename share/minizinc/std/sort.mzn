include "fzn_sort.mzn";
include "fzn_sort_reif.mzn";

/** @group globals.sort
  Requires that the multiset of values in \a xs are the same as the
  multiset of values in \a ys but \a ys is in sorted order.
*/
predicate sort(array [$$E] of var $$T: xs, array [$$F] of var $$T: ys) =
  assert(
    length(xs) == length(ys),
    "sort: xs and ys must be same sized arrays",
    fzn_sort(index2int(enum2int(xs)), index2int(enum2int(ys))),
  );
