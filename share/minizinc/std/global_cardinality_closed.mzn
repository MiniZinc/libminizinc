/** @group globals.counting
  Requires that the number of occurences of \p i in \a x is \a counts[\p i].

  The elements of \a x must take their values from \a cover.
*/
predicate global_cardinality_closed(array[int] of var int: x,
                                    array[int] of int: cover,
                                    array[int] of var int: counts) =
    assert(index_set(cover) = index_set(counts),
        "global_cardinality_closed: " ++
        "cover and counts must have identical index sets",
    forall(i in index_set(x))( x[i] in { d | d in cover } )
    /\
    global_cardinality(x, cover, counts)
    /\
    % Implied constraint
    length(x) = sum(counts)
    );


include "global_cardinality.mzn";
