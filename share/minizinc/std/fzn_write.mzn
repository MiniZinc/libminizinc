/** @group globals.array
   Creates a new array \a O from an input array \a I with a change at position \a i to take value \a v

   \a I is an array of integers
   \a O is an array of integers with same index set as \a I
   \a i is an index for \a I
   \a v is an integer value 

*/
predicate fzn_write(array[int] of var int: I, var int: i, var int: v,
                    array[int] of var int: O) =
    forall(j in index_set(I))
          (O[j] = if j = i then v else I[j] endif);

%-----------------------------------------------------------------------------%
