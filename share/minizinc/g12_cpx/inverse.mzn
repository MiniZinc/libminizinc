predicate inverse_offsets(array[int] of var int: f, int: foff,
                          array[int] of var int: invf, int: invfoff);

predicate inverse(array[int] of var int: f, array[int] of var int: invf) =
    inverse_offsets(f, min(index_set(invf)), invf, min(index_set(f)));
