predicate fzn_global_cardinality_low_up_closed(array[int] of var int: x,
                                           array[int] of int: cover,
				           array[int] of int: lbound,
                                           array[int] of int: ubound) =
    forall(i in index_set(x))( x[i] in { d | d in array2set(cover) } )
    /\
    global_cardinality_low_up(x, cover, lbound, ubound)
    /\
    % Implied condition
    length(x) in sum(lbound)..sum(ubound);


include "global_cardinality_low_up.mzn";
