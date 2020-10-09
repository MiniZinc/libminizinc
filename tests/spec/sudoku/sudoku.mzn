/***
--- !Test
extra_files: [sudoku_1_16x16.dzn]
solvers: [cbc, chuffed]
check_against: [cbc, chuffed]
expected: !Result
  solution: !Solution {}
--- !Test
extra_files: [sudoku_2_16x16.dzn]
solvers: [cbc, chuffed]
check_against: [cbc, chuffed]
expected: !Result
  solution: !Solution {}
--- !Test
extra_files: [sudoku_3_16x16.dzn]
solvers: [cbc, chuffed]
check_against: [cbc, chuffed]
expected: !Result
  solution: !Solution {}
--- !Test
extra_files: [sudoku_4_16x16.dzn]
solvers: [cbc, chuffed]
check_against: [cbc, chuffed]
expected: !Result
  solution: !Solution {}
--- !Test
extra_files: [sudoku_5_16x16.dzn]
solvers: [cbc, chuffed]
check_against: [cbc, chuffed]
expected: !Result
  solution: !Solution {}
***/


%-----------------------------------------------------------------------------%
% Sudoku for squares of arbitrary size N = (S x S)
%-----------------------------------------------------------------------------%

%int: S=3;
%int: N = S * S;
%array[1..N,1..N] of int: puzzle_input=[|
%0, 0, 0, 0, 0, 0, 0, 0, 0|
%0, 6, 8, 4, 0, 1, 0, 7, 0|
%0, 0, 0, 0, 8, 5, 0, 3, 0|
%0, 2, 6, 8, 0, 9, 0, 4, 0|
%0, 0, 7, 0, 0, 0, 9, 0, 0|
%0, 5, 0, 1, 0, 6, 3, 2, 0|
%0, 4, 0, 6, 1, 0, 0, 0, 0|
%0, 3, 0, 2, 0, 7, 6, 9, 0|
%0, 0, 0, 0, 0, 0, 0, 0, 0|
%|];

int: S;
int: N=S*S;
array[1..N,1..N] of int: puzzle_input;

%int: S=4;
%int: N=S*S;
%array[1..N,1..N] of int: puzzle_input=[|
%13, 4, 0, 16, 0, 2, 0, 0, 0, 0, 0, 0, 11, 0, 14, 10|
%9, 12, 14, 5, 7, 0, 0, 15, 0, 0, 0, 0, 0, 2, 1, 0|
%2, 15, 0, 6, 16, 0, 10, 9, 0, 0, 0, 0, 0, 5, 3, 0|
%0, 0, 1, 0, 5, 0, 14, 0, 0, 0, 0, 0, 9, 6, 0, 0|
%0, 0, 0, 0, 10, 12, 0, 5, 0, 0, 13, 0, 0, 0, 6, 8|
%0, 0, 0, 0, 8, 0, 9, 0, 2, 0, 0, 14, 16, 0, 0, 11|
%0, 0, 0, 0, 0, 0, 6, 7, 9, 0, 0, 0, 5, 15, 13, 12|
%0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 12, 2, 0, 0, 3|
%16, 9, 15, 10, 0, 0, 0, 4, 0, 6, 0, 2, 0, 0, 0, 0|
%0, 13, 0, 3, 0, 11, 0, 0, 0, 4, 1, 16, 0, 0, 0, 0|
%0, 0, 8, 12, 6, 7, 16, 0, 0, 5, 0, 0, 0, 0, 0, 0|
%0, 2, 11, 0, 1, 0, 0, 0, 10, 13, 12, 0, 0, 0, 0, 0|
%4, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 9|
%3, 6, 0, 0, 0, 0, 0, 0, 12, 1, 11, 8, 14, 0, 0, 5|
%0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 7, 10, 15, 11, 0, 4|
%0, 11, 10, 1, 0, 0, 0, 0, 0, 0, 15, 9, 6, 0, 7, 0|];

%int: S=5;
%int: N=S*S;
%array[1..N,1..N] of int: puzzle_input=[|
%0, 12, 0, 25, 10, 0, 0, 0, 11, 24, 2, 0, 0, 0, 14, 23, 0, 9, 0, 0, 19, 0, 4, 0, 3|
%0, 14, 0, 4, 1, 0, 15, 21, 2, 0, 0, 0, 13, 12, 0, 0, 0, 20, 24, 0, 0, 0, 8, 18, 5|
%0, 18, 0, 9, 17, 0, 0, 6, 0, 5, 0, 0, 21, 24, 0, 19, 0, 0, 10, 16, 0, 0, 23, 0, 0|
%16, 23, 0, 0, 0, 0, 25, 10, 14, 0, 22, 0, 0, 1, 6, 0, 7, 0, 0, 0, 0, 0, 20, 24, 0|
%0, 0, 0, 0, 0, 7, 0, 0, 0, 12, 19, 0, 0, 15, 0, 0, 5, 6, 0, 0, 22, 0, 0, 16, 0|
%0, 17, 10, 0, 0, 0, 0, 0, 0, 11, 0, 12, 24, 0, 0, 6, 21, 0, 18, 0, 0, 19, 0, 0, 0|
%0, 13, 0, 19, 0, 0, 0, 0, 21, 1, 10, 15, 22, 11, 0, 0, 16, 0, 25, 0, 23, 0, 5, 0, 0|
%9, 22, 0, 11, 15, 12, 18, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 23, 10, 17, 0, 0, 0|
%0, 0, 0, 2, 0, 0, 0, 24, 0, 25, 0, 13, 0, 23, 0, 0, 0, 0, 4, 0, 15, 9, 22, 0, 21|
%0, 24, 21, 3, 23, 6, 0, 15, 0, 0, 0, 25, 18, 0, 17, 0, 0, 0, 9, 0, 14, 12, 0, 8, 0|
%4, 15, 0, 0, 5, 0, 11, 0, 1, 0, 9, 16, 14, 10, 0, 0, 6, 17, 0, 0, 0, 13, 0, 3, 19|
%0, 0, 0, 10, 0, 0, 0, 8, 19, 0, 0, 6, 0, 17, 0, 0, 0, 16, 0, 25, 4, 1, 0, 5, 0|
%0, 1, 19, 13, 0, 0, 0, 25, 0, 0, 3, 2, 11, 0, 0, 12, 0, 23, 0, 0, 0, 21, 0, 0, 0|
%0, 21, 11, 0, 0, 17, 7, 0, 5, 3, 0, 23, 8, 0, 0, 0, 1, 0, 2, 13, 0, 25, 0, 10, 14|
%0, 0, 23, 0, 0, 4, 0, 0, 0, 14, 7, 0, 0, 25, 0, 0, 0, 24, 0, 9, 2, 0, 0, 0, 0|
%0, 0, 0, 7, 18, 11, 5, 0, 0, 22, 0, 14, 0, 0, 0, 1, 0, 2, 20, 0, 3, 0, 19, 13, 0|
%6, 10, 2, 14, 0, 0, 0, 20, 0, 0, 0, 0, 23, 7, 4, 0, 12, 25, 3, 0, 0, 16, 21, 17, 0|
%15, 0, 0, 0, 11, 0, 21, 0, 0, 0, 0, 0, 17, 8, 22, 5, 18, 0, 0, 0, 7, 24, 0, 0, 0|
%0, 0, 13, 0, 21, 1, 0, 23, 24, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 8|
%0, 9, 0, 12, 0, 0, 0, 14, 7, 0, 0, 24, 0, 19, 20, 16, 0, 0, 0, 10, 0, 0, 0, 25, 18|
%21, 25, 1, 5, 0, 0, 0, 0, 0, 2, 0, 17, 0, 14, 18, 0, 4, 0, 7, 0, 0, 0, 0, 0, 16|
%8, 19, 0, 15, 0, 0, 9, 0, 20, 21, 25, 0, 3, 0, 11, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0|
%0, 0, 0, 0, 2, 0, 0, 7, 0, 0, 0, 22, 0, 0, 9, 17, 14, 0, 23, 0, 8, 4, 0, 0, 6|
%0, 4, 0, 0, 6, 18, 0, 3, 10, 17, 8, 0, 0, 0, 0, 0, 0, 0, 0, 12, 21, 22, 0, 0, 7|
%0, 0, 17, 0, 0, 5, 0, 4, 25, 0, 12, 19, 20, 13, 0, 0, 0, 0, 0, 1, 0, 14, 9, 0, 11|];

array[1..N*N] of var 1..N: puzzle;

constraint forall (i in 1..N, j in 1..N) (
    if (puzzle_input[i,j]==0) then true
    else puzzle[(i-1)*N+j]==puzzle_input[i,j]
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
    forall(i in 1..N)( alldiff(j in 1..N)( puzzle[(i-1)*N+j] ))
    /\
    forall(j in 1..N)( alldiff(i in 1..N)( puzzle[(i-1)*N+j] ))
    /\
    forall(i,j in 1..S)
        ( alldiff(p,q in 1..S)( puzzle[(S*(i-1)+p-1)*N+(S*(j-1)+q)] ))
    ;


solve satisfy;

%-----------------------------------------------------------------------------%
%
% The data for the puzzle that causes satz to make 1 backtrack (normally none
% are made).
%

