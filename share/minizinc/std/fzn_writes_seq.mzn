/** @group globals.array
    Creates a new array \a O from an input array \a I with a sequence of changes at positions \a P to take values \a V

    \a I is an array of integers
    \a O is an array of integers with same index set as \a I
    \a P is an array of index values in \a I
    \a V is an array of integer values 
*/
predicate fzn_writes_seq(array[int] of var int: I, array[int] of var int: P,
                         array[int] of var int: V, array[int] of var int: O) =
    forall (i in index_set(I)) (
        let {
            array ... of var int: Vi = reverse(V) ++ I[i];
            array ... of var bool: Pi = reverse([P[j] == i | j in index_set(V)]) ++ [true]; 
        } in O[i] = Vi[arg_max(Pi)]
    );

%-----------------------------------------------------------------------------%
