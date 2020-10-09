/***
!Test
expected:
- !Result
  solution: !Solution
    q:
    - [1, 4, 5, 2, 3]
    - [3, 2, 1, 5, 4]
    - [4, 5, 3, 1, 2]
    - [5, 3, 2, 4, 1]
    - [2, 1, 4, 3, 5]
***/

%% Bennett quasigroup existence problem (QG5).
%%
%% A quasigroup is just a groupoid whose "multiplication table" is a
%% Latin square: each row and each column is a permutation of the
%% elements. Bennett's equation (not invented by Frank Bennett but
%% studied by him) is (yx.y)y = x which forces the quasigroup to have
%% interesting properties such as being orthogonal to certain of its
%% conjugates. Idempotent ones are more important than the others.
%%
%% Bennett quasigroups exist for all positive N except for 2, 6, 10
%% and with the possible exceptions of 14, 18, 26, 30, 38, 42 and 158.
%%
%% Idempotent ones exist for all positive N not in {2, 3, 4, 6, 9, 10,
%% 12, 13, 14, 15, 16} except possibly for N in {18, 20, 22, 24, 26,
%% 28, 30, 34, 38, 39, 42, 44, 46, 51, 52, 58, 60, 62, 66, 68, 70, 72,
%% 74, 75, 76, 86, 87, 90, 94, 96, 98, 99, 100, 102, 106, 108, 110,
%% 114, 116, 118, 122, 132, 142, 146, 154, 158, 164, 170, 174}
%%
%% The existence of smallish open problems makes this algebraic
%% question attractive from the viewpoint of automated reasoning and
%% constraint satisfaction.
%%
%% Reference:
%% F. E. Bennett
%% Quasigroups
%% C. J. Colbourn (ed)
%% The CRC Handbook of Combinatorial Designs
%% Chapter 36, pp .424-429.

include "globals.mzn";

int: N;

array[1..N,1..N] of var 1..N: q;

constraint
  forall( x in 1..N )
        ( q[x, x] = x );

constraint
  forall( x in 1..N )
        ( alldifferent([q[x,y] | y in 1..N]) );

constraint
  forall( y in 1..N )
        ( alldifferent([q[x,y] | x in 1..N]) );

constraint
  forall( x in 1..N )
        ( q[x, 1] < x + 2 );

constraint
  forall( x in 1..N, y in 1..N )
        ( q[q[q[y, x], y], y] = x );

solve :: int_search(array1d(1..N*N, q), input_order, indomain, complete)
    satisfy;

output ["Bennett quasigroup of size " ++ show(N) ++ ":\n"] ++
       [ if y=1 then "\n  " else "  " endif ++ show(q[x,y]) | x,y in 1..N ] ++
       [ "\n" ];

N = 5;  % Solutions also exist for N=7, N=8 and N=11.
