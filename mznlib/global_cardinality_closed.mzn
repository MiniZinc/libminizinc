%-----------------------------------------------------------------------------%
% Requires that the number of occurences of 'i' in 'x' is 'counts[i]'.
% The elements of 'x' must take their values from 'cover'.
%-----------------------------------------------------------------------------%

predicate global_cardinality_closed(array[int] of var int: x,
                                    array[int] of int: cover,
                                    array[int] of var int: counts) =
    assert(index_set(cover) = index_set(counts),
        "global_cardinality_closed: " ++
        "cover and counts must have identical index sets",
    ilogcp_global_cardinality_closed(counts,cover,x)
    );

predicate ilogcp_global_cardinality_closed(array[int] of var int: counts,
                                    array[int] of int: cover,
                                    array[int] of var int: x);

include "global_cardinality.mzn";

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
