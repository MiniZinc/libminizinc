%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_lesseq_bool(array[int] of var bool: x,
                          array[int] of var bool: y) =
%   if (min(card(index_set(x)), card(index_set(y))) <= 25) then
%     let { int: size = min(card(index_set(x)), card(index_set(y)));
%     } in
%       sum(i in 0..size-1)(pow(2, (size-1-i)) * bool2int(x[i+min(index_set(x))]))
%         <= sum(i in 0..size-1)(pow(2, (size-1-i)) * bool2int(y[i+min(index_set(y))]))
%   else
%    my_trace ("lex_lesseq_bool(\(x), \(y))") /\
    let { int: lx = min(index_set(x)),
          int: ux = max(index_set(x)),
          int: ly = min(index_set(y)),
          int: uy = max(index_set(y)),
          int: size = min(ux - lx, uy - ly),
          array[0..size+1] of var bool: b }
          % b[i] is true if the lexicographical order holds from position i on.
    in
    b[0]
    /\
    forall(i in 0..size) (
        b[i] -> ( ( ( x[lx + i] <= y[ly + i] ) )  /\
 %          bool2int(b[i]) + bool2int(x[lx + i]) + (1-bool2int(y[ly + i])) <= 2 /\
                 
%        ( b[i] -> 
( x[lx + i] <  y[ly + i] \/ b[i+1] ) )
%        /\  ( bool2int(b[i]) <= bool2int(x[lx + i] <  y[ly + i]) + bool2int(b[i+1]) )    /\
 %      bool2int(b[i]) + (1-bool2int(x[lx + i])) + (1-bool2int(y[ly + i])) + (1-bool2int(b[i+1])) <= 3
 %      /\ bool2int(b[i]) + bool2int(x[lx + i]) + bool2int(y[ly + i]) + (1-bool2int(b[i+1])) <= 3
    %% This guy is dominated by the 1st one above but helps:
  %     /\ bool2int(b[i]) + bool2int(x[lx + i]) + (1-bool2int(y[ly + i])) + (1-bool2int(b[i+1])) <= 3
    )
    /\ b[size+1] = (ux-lx <= uy-ly)
%  endif
;

%     forall(i in 0..size) (
%        ( b[i] == ( x[lx + i] <= y[ly + i] ) )
%                  /\
%        if i < size then 
%         ( b[i] == ( x[lx + i] <  y[ly + i] \/ b[i+1]
%                ) ) else true endif
%     );

predicate lex_lesseq_bool_reif(array[int] of var bool: x,
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

predicate lex_lesseq_bool_imp(array[int] of var bool: x,
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


predicate lex_leq_bool(array[int] of var bool: x,
                       array[int] of var bool: y) =
          lex_lesseq(x, y);

%-----------------------------------------------------------------------------%
