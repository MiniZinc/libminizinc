/***
!Test
solvers: [gecode, chuffed]
expected:
- !Result
  solution: !Solution
    objective: -2147483646
    var1: 1
    var2: -2147483646
    var3: -2147483646
- !Result
  solution: !Solution
    objective: -500000000
    var1: 1
    var2: -500000000
    var3: -500000000
***/

% Regression test for bug #318: this model as causing mzn2fzn to abort because
% the second argument of the element constraint below wasn't flat by the
% optimisation pass (or the output pass if optimisation was disabled).

include "globals.mzn";

var int: var1;
var int: var2;
var int: var3;

constraint element(var1,[var2, (var2+1) ],var3);

solve minimize var3;

output ["Ok\n"];
