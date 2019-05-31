/** @group globals.array
   Creates a new array \a O from an input array \a I with a sequence of changes at positions \a P to take values \a V

   \a I is an array of integers
   \a O is an array of integers with same index set as \a I
   \a P is an array of index values in \a I
   \a V is an array of integer values 

*/
predicate fzn_writes_seq(array[int] of var int: I, var int: i, var int: v,
                         array[int] of var int: O) =
    forall(i in index_set(I))
          (if forall(j in index_set(P))(P[j] != i) then
   	      O[i] = I[i]
	   else true endif) /\
    forall(j in index_set(P))
          (if forall(k in index_set(P) where k > j)(P[k] != P[j]) then
	             O[P[j]] = V[j]
	   else true endif);

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
