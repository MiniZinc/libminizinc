/** @group globals.counting
  Requires that the number of occurences of \p i in \a x is \a counts[\p i].

  The elements of \a x must take their values from \a cover.
*/
predicate fzn_global_cardinality_closed(array[int] of var int: x,
                                    array[int] of int: cover,
                                    array[int] of var int: counts) =
    forall(i in index_set(x))( x[i] in { d | d in cover } )
    /\
    global_cardinality(x, cover, counts)
    ;


include "global_cardinality.mzn";
