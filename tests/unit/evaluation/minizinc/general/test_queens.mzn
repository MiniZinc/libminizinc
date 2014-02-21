% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
% RUNS ON mzn_lazyfd
% RUNS ON mzn_mer_lazyfd
% RUNS ON mzn_cd_lazyfd
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
