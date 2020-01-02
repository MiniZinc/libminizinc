/***
!Test
expected:
- !Result
  solution: !Solution
    lastin: 1
    succ: [2, 1, 1, 1, 1, 1, 1, 1, 1, 1]
- !Result
  solution: !Solution
    lastin: 4
    succ: [1, 3, 1, 1, 1, 1, 1, 1, 1, 1]
- !Result
  solution: !Solution
    lastin: 10
    succ: [1, 2, 3, 4, 5, 6, 7, 8, 10, 2]
***/

% Regression test for a problem with the "linear" MiniZinc redefinitions.
% The signature of array_var_bool_element/3 in the redefintions was
% incorrect leading to an internal error in mzn2fzn.

array[1..10] of var 1..10: succ;
array[1..10] of var bool: ins = [ succ[i] != i | i in 1..10];
var 1..11: firstin = min([ 10+1 + bool2int(ins[i])*(i-10-1) | i in 1..10]);
var 1..10: lastin;

constraint ins[lastin];

solve satisfy;

output ["Ok\n"];
