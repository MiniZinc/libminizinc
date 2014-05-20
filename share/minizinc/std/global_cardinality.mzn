%-----------------------------------------------------------------------------%
% Requires that the number of occurrences of 'cover[i]' in 'x' is 'counts[i]'.
%-----------------------------------------------------------------------------%

include "count.mzn";

predicate global_cardinality(array[int] of var int: x,
	  		     array[int] of int: cover,
                             array[int] of var int: counts) =
    assert(index_set(cover) = index_set(counts),
        "global_cardinality: cover and counts must have identical index sets",
    forall(i in index_set(cover))( count(x, cover[i], counts[i]) )
    /\
    % Implied constraint
    length(x) >= sum(counts)
    );

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
