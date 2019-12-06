%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_lesseq_int_reif(array[int] of var int: x,
                              array[int] of var int: y,
                              var bool: c) =
    let { int: lx = min(index_set(x)),
          int: ux = max(index_set(x)),
          int: ly = min(index_set(y)),
          int: uy = max(index_set(y)),
          int: size = min(ux - lx, uy - ly),
          array[0..size+1] of var bool: b }
          % b[i] is true if the lexicographical order holds from position i on.
    in
    (c <-> b[0])
    /\
    forall(i in 0..size) (
        b[i] = ( x[lx + i] <= y[ly + i]
                 /\
                 (x[lx + i] <  y[ly + i] \/ b[i+1])
               )
    )
    /\
    b[size + 1] = (ux - lx <= uy - ly)
    ;


%-----------------------------------------------------------------------------%
