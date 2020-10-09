include "arg_max.mzn";

predicate fzn_writes_seq_reif(array[int] of var int: I, array[int] of var int: P,
                              array[int] of var int: V, array[int] of var int: O,
                              var bool: b) =
    b <-> forall (i in index_set(I)) (
        let {
            array[1..length(V)+1] of var int: Vi = array1d(1..length(V)+1, reverse(V) ++ [I[i]]);
            array[1..length(V)+1] of var bool: Pi = array1d(1..length(V)+1, reverse([P[j] == i | j in index_set(V)]) ++ [true]); 
        } in O[i] = Vi[arg_max(Pi)]
    );

%-----------------------------------------------------------------------------%
