/***
!Test
solvers: [gecode, chuffed] # Too slow on cbc
expected:
- !Result
  solution: !Solution
    p: [1, 9, 5, 16, 3, 7, 15, 2, 10, 6, 17, 30, 34, 26, 13, 21, 32, 19, 8, 4, 12, 23, 36, 28, 20, 31, 27, 35, 24, 11, 22, 18, 29, 33, 25, 14]
***/

% knights.mzn
% Ralph Becket
% vim: ft=zinc ts=4 sw=4 et
% Tue Aug 26 14:24:28 EST 2008
%
% Find a closed knight's tour of a chessboard (every square is visited exactly
% once, the tour forms a loop).

include "globals.mzn";

    % n is the length of side of the chessboard.
    %
int: n = 6;

    % The ith square (r, c) on the path is given by p[i] = (r - 1) * n + c.
    %
int: nn = n * n;
set of int: sq = 1..nn;
array [sq] of var sq: p;

set of int: row = 1..n;
set of int: col = 1..n;

    % Break some symmetry by specifying the first and last moves.
    %
constraint p[1]  = 1;
constraint p[2]  = n + 3;
constraint p[nn] = 2 * n + 2;

    % All points along the path must be unique.
    %
constraint alldifferent(p);

array [sq] of set of sq: neighbours =
    [   { n * (R - 1) + C
        |
            i in 1..8,
            R in {R0 + [-1, -2, -2, -1,  1,  2,  2,  1][i]},
            C in {C0 + [-2, -1,  1,  2,  2,  1, -1, -2][i]}
            where R in row /\ C in col
        }
    |   R0 in row, C0 in col
    ];

constraint forall (i in sq where i > 1) (p[i] in neighbours[p[i - 1]]);

solve
    :: int_search(
        p,
        input_order,
        indomain_min,
        complete
    )
    satisfy;
% It has been observed that Warnsdorf's heuristic of choosing the next
% square as the one with the fewest remaining neighbours leads almost 
% directly to a solution.  How might we express this in MiniZinc?

output ["p = " ++ show(p) ++ ";\n"];

% Invert the path to show the tour.
% 
% array [sq] of var sq: q;
% 
% constraint forall (i in sq) (q[p[i]] = i);
% 
% output  [   show(q[i]) ++ if i mod n = 0 then "\n" else " " endif
%         |   i in sq
%         ] ++
%         [   "\n"
%         ];

