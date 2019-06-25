include "fzn_writes.mzn";
include "fzn_writes_reif.mzn";

/** @group globals.array
    Creates a new array \a O from an input array \a I with a simultaneous change at positions \a P to values \a V

    \a I is an array of integers
    \a O is an array of integers with same index set as \a I
    \a P is an array of index values in \a I
    \a V is an array of integer values 
*/
predicate writes(array[int] of var int: I, array[int] of var int: P,
                 array[int] of var int: V, array[int] of var int: O) =
    assert(index_set(O) = index_set(I),"writes: index set of I must be same as O") /\
    assert(index_set(P) = index_set(V),"writes: index set of P must be same as V") /\
    fzn_writes(I, P, V, O);

function array[int] of var int: writes(array[int] of var int: I,
                                       array[int] of var int: P,
                                       array[int] of var int: V) = 
    assert(index_set(P) = index_set(V),"writes: index set of P must be same as V") /\
    let { array[index_set(I)] of var int: O;
          constraint fzn_writes(I,P,V,O); }
    in O;

%-----------------------------------------------------------------------------%