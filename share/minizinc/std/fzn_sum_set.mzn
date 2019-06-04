/** @group globals
  Requires that the sum of the weights \a ws[\p i1]..\a ws[\p iN] equals \a s, 
  where \a vs[\p i1]..\a vs[\p iN] are the elements appearing in set \a x
*/
predicate fzn_sum_set(array[int] of int: vs, array[int] of int: ws,
                      var set of int: x, var int: s) =
    s == sum(j in index_set(vs)) ( bool2int(j in x) * ws[j] );
