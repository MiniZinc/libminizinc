%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_less_int(array[int] of var int: x,
                           array[int] of var int: y) =
    assert(length(x) == length(y),
        "lex_less_int(\(x), \(y)): arrays of different lengths")
    /\
    fzn_lex_less_int__MIP(x, y)
    ;

predicate fzn_lex_less_int__MIP(array[int] of var int: x,
                                array[int] of var int: y) =
    let { int: lx = min(index_set(x)),
          int: ux = max(index_set(x)),
          int: ly = min(index_set(y)),
          int: uy = max(index_set(y)),
          int: size = min(ux - lx, uy - ly),
          array[0..size] of var 0..1: fEQ;
          array[0..size] of var 0..1: fLT;
    } in
        sum(fLT) == 1
        /\
        fEQ[0] + fLT[0] == 1
        /\
        forall(i in 1..size) (
            fEQ[i-1] == fEQ[i] + fLT[i]
        )
        /\
        forall(i in 0..size) (
            aux_int_eq_if_1(x[lx+i], y[ly+i], fEQ[i])
            /\
            aux_int_lt_if_1(x[lx+i], y[ly+i], fLT[i])
        )
    ;

%-----------------------------------------------------------------------------%
