% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd
% RUNS ON mzn_mer_lp
% RUNS ON zinc_fdic_mznlib
% RUNS ON zinc_mip_hl
% RUNS ON minizinc_cpx
% RUNS ON minizinc_fd
%
%-----------------------------------------------------------------------------%
% Sudoku for squares of arbitrary size N = (S x S)
%-----------------------------------------------------------------------------%

int: S;
int: N = S * S;


array[1..N,1..N] of var 1..N: puzzle;


include "alldifferent.mzn";

    % All cells in a row, in a column, and in a subsquare are different.
constraint
    forall(i in 1..N)( alldifferent(j in 1..N)( puzzle[i,j] ))
    /\
    forall(j in 1..N)( alldifferent(i in 1..N)( puzzle[i,j] ))
    /\
    forall(i,j in 1..S)
        ( alldifferent(p,q in 1..S)( puzzle[S*(i-1)+p, S*(j-1)+q] ));


solve satisfy;


output [ "sudoku:\n" ] ++
    [ show(puzzle[i,j]) ++
        if j = N then
            if i mod S = 0 /\ i < N then "\n\n" else "\n" endif
        else
            if j mod S = 0 then "  " else " " endif
        endif
    | i,j in 1..N ];

%-----------------------------------------------------------------------------%
%
% The data for the puzzle that causes satz to make 1 backtrack (normally none
% are made).
%

S=3;
puzzle=[|
_, _, _, _, _, _, _, _, _|
_, 6, 8, 4, _, 1, _, 7, _|
_, _, _, _, 8, 5, _, 3, _|
_, 2, 6, 8, _, 9, _, 4, _|
_, _, 7, _, _, _, 9, _, _|
_, 5, _, 1, _, 6, 3, 2, _|
_, 4, _, 6, 1, _, _, _, _|
_, 3, _, 2, _, 7, 6, 9, _|
_, _, _, _, _, _, _, _, _|
|];

