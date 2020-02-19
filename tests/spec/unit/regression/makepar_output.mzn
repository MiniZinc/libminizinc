/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: '1..1'
***/

% Used to crash due to trying to use the var version of binops in output
% Fixed in b4510b996be58e2831daab37a18ccecc8a36ba6d

var 1..10: x;
var 1..10: y;

output [show(x..y)];