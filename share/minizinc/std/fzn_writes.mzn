/** @group globals.array
    Creates a new array \a O from an input array \a I with a simultaneous change at positions \a P to values \a V

    \a I is an array of integers
    \a O is an array of integers with same index set as \a I
    \a P is an array of index values in \a I
    \a V is an array of integer values 
*/
predicate fzn_writes(array[int] of var int: I, array[int] of var int: P,
                     array[int] of var int: V, array[int] of var int: O) =
    forall(j in index_set(P))(O[P[j]] = V[j]) /\
	  forall(i in index_set(I))
        (if forall(j in index_set(P))(P[j] != i) then
            O[i] = I[i]
        else true endif);

%-----------------------------------------------------------------------------%
