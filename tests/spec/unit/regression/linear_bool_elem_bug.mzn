/***
!Test
expected:
- !Result
  solution: !Solution
    firstin: 1
    lastin: 1
- !Result
  solution: !Solution
    firstin: 1
    lastin: 2
- !Result
  solution: !Solution
    firstin: 2
    lastin: 2
- !Result
  solution: !Solution
    firstin: 1
    lastin: 3
- !Result
  solution: !Solution
    firstin: 2
    lastin: 3
- !Result
  solution: !Solution
    firstin: 3
    lastin: 3
***/

% Regression test for a problem with the "linear" MiniZinc redefinitions.
% The signature of array_var_bool_element/3 in the redefintions was
% incorrect leading to an internal error in mzn2fzn.

int: n=3;
array[1..n] of var 1..n: succ;
array[1..n] of var bool: ins = [ succ[i] != i | i in 1..n];
var 1..n+1: firstin;
constraint firstin == min([ n+1 + bool2int(ins[i])*(i-n-1) | i in 1..n]);
var 1..n: lastin;

constraint ins[lastin];

solve satisfy;

output ["Ok\n"];
