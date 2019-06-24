predicate fzn_write(array[int] of var int: I, var int: i, var int: v,
                    array[int] of var int: O) =
    forall(j in index_set(I))
          (O[j] = if j = i then v else I[j] endif);

%-----------------------------------------------------------------------------%
