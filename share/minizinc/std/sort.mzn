include "fzn_sort.mzn";

/** @group globals.sort
  Requires that the multiset of values in \a x are the same as the
  multiset of values in \a y but \a y is in sorted order.
*/
predicate sort(array[int] of var int: x, array[int] of var int: y) =
    assert(card(index_set(x)) == card(index_set(y)),
        "sort: x and y must be same sized arrays",
        fzn_sort(x,y)
    );
