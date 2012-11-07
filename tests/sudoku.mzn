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

int: S=3;
int: N = S * S;


array[1..N,1..N] of var 1..N: puzzle;
array[1..N,1..N] of int: puzzle_input=[|
0, 0, 0, 0, 0, 0, 0, 0, 0|
0, 6, 8, 4, 0, 1, 0, 7, 0|
0, 0, 0, 0, 8, 5, 0, 3, 0|
0, 2, 6, 8, 0, 9, 0, 4, 0|
0, 0, 7, 0, 0, 0, 9, 0, 0|
0, 5, 0, 1, 0, 6, 3, 2, 0|
0, 4, 0, 6, 1, 0, 0, 0, 0|
0, 3, 0, 2, 0, 7, 6, 9, 0|
0, 0, 0, 0, 0, 0, 0, 0, 0|
|];



constraint forall (i in 1..N, j in 1..N) (
    if (puzzle_input[i,j]==0) then true
    else puzzle[i,j]==puzzle_input[i,j]
    endif
  );

ann: total;

function var int: eq_const(var int: x, int: d) ::total =
  if d in dom(x) then let {
    array[int] of var int: i2ax = int2a(x)
  } in i2ax[d] else 0 endif;

function array[int] of var int: int2a(var int: x) ::total =
  let {
    array[dom(x)] of var 0..1: a;
    constraint int_lin_eq([1|i in index_set(a)],a,1);
    constraint int_lin_eq([i|i in index_set(a)]++[-1],a++[x],0);
  } in a;

predicate alldiff(array[int] of var int: x) =
  forall (d in dom_array(x)) (
    int_lin_le([1|i in index_set(x)],[eq_const(x[i],d) | i in index_set(x)],1)
  );

predicate alldiff_builtin(array[int] of var int: x);

    % All cells in a row, in a column, and in a subsquare are different.
constraint
    forall(i in 1..N)( alldiff(j in 1..N)( puzzle[i,j] ))
    /\
    forall(j in 1..N)( alldiff(i in 1..N)( puzzle[i,j] ))
    /\
    forall(i,j in 1..S)
        ( alldiff(p,q in 1..S)( puzzle[S*(i-1)+p, S*(j-1)+q] ))
    ;


solve satisfy;

%-----------------------------------------------------------------------------%
%
% The data for the puzzle that causes satz to make 1 backtrack (normally none
% are made).
%

