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

/** @group globals.sort
  Return a multiset of values that is the same as the
  multiset of values in \a x but in sorted order.
*/
function array[int] of var int: sort(array[int] of var int: x) ::promise_total =
    let {
      array[1..length(x)] of var lb_array(x)..ub_array(x): y;
      constraint fzn_sort(x,y);
    } in y;
