predicate fzn_lex_lesseq_bool_reif(array[int] of var bool: x,
                                   array[int] of var bool: y,
                                   var bool: c) =
    let { int: lx = min(index_set(x)),
          int: ux = max(index_set(x)),
          int: ly = min(index_set(y)),
          int: uy = max(index_set(y)),
          int: size = max(ux - lx, uy - ly),
          array[0..size+1] of var bool: b }
          % b[i] is true if the lexicographical order holds from position i on.
    in
    (c <-> b[0])
    /\
    forall(i in 0..size) (
        ( b[i] -> ( x[lx + i] <= y[ly + i] ) )  /\
           bool2int(b[i]) + bool2int(x[lx + i]) + (1-bool2int(y[ly + i])) <= 2
                 /\
        ( b[i] -> ( x[lx + i] <  y[ly + i] \/ b[i+1] ) )                 /\
       bool2int(b[i]) + (1-bool2int(x[lx + i])) + (1-bool2int(y[ly + i])) + (1-bool2int(b[i+1])) <= 3
       /\ bool2int(b[i]) + bool2int(x[lx + i]) + bool2int(y[ly + i]) + (1-bool2int(b[i+1])) <= 3
       /\ bool2int(b[i]) + bool2int(x[lx + i]) + (1-bool2int(y[ly + i])) + (1-bool2int(b[i+1])) <= 3
    )
    /\ b[size+1] = (ux-lx <= uy-ly)
%  endif
;

predicate fzn_lex_lesseq_bool_imp(array[int] of var bool: x,
                                  array[int] of var bool: y,
                                  var bool: c) =
    let { int: lx = min(index_set(x)),
          int: ux = max(index_set(x)),
          int: ly = min(index_set(y)),
          int: uy = max(index_set(y)),
          int: size = max(ux - lx, uy - ly),
          array[0..size+1] of var bool: b }
          % b[i] is true if the lexicographical order holds from position i on.
    in
    (c -> b[0])
    /\
    forall(i in 0..size) (
        ( b[i] -> ( x[lx + i] <= y[ly + i] ) )  /\
           bool2int(b[i]) + bool2int(x[lx + i]) + (1-bool2int(y[ly + i])) <= 2
                 /\
        ( b[i] -> ( x[lx + i] <  y[ly + i] \/ b[i+1] ) )                 /\
       bool2int(b[i]) + (1-bool2int(x[lx + i])) + (1-bool2int(y[ly + i])) + (1-bool2int(b[i+1])) <= 3
       /\ bool2int(b[i]) + bool2int(x[lx + i]) + bool2int(y[ly + i]) + (1-bool2int(b[i+1])) <= 3
       /\ bool2int(b[i]) + bool2int(x[lx + i]) + (1-bool2int(y[ly + i])) + (1-bool2int(b[i+1])) <= 3
    )
    /\ b[size+1] = (ux-lx <= uy-ly)
;
