/***
!Test
solvers:
- gecode
- chuffed
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    q: [9, 13, 2, 8, 11, 20, 10, 6, 19, 15, 18, 12, 7, 16, 4, 17, 14, 5, 3, 1]
- !Result
  status: SATISFIED
  solution: !Solution
    q: [9, 7, 10, 14, 11, 15, 4, 12, 5, 13, 1, 3, 6, 8, 2, 17, 19, 16, 18, 20]
***/

% Copied from the queens.mzn benchmark.
%
% In some very rare cases some literals in MiniSAT are labeled during 
% backjumping but never actually propagated 
% (if invoke is never entered again), 
% which leads to the output of flatzinc showing unfixed int variables.
%
%

int: n = 20;               % The number of queens.

array [1..n] of var 1..n: q;

predicate 
    noattack(int: i, int: j, var int: qi, var int: qj) =
    qi     != qj     /\
    qi + i != qj + j /\
    qi - i != qj - j;

constraint
    forall (i in 1..n, j in i+1..n) (
        noattack(i, j, q[i], q[j])
    );

solve satisfy;

output ["% ", show(n), "-queens:\n"] ++
       ["% "] ++ [show(q[i]) ++ " " | i in 1..n] ++
       ["\n%\n"] ++
       ["q = ", show(q), ";\n"];
