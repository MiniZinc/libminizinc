/***
!Test
expected:
- !Result
  solution: !Solution
    objective: 1
    var1: 1
    var2: 2
    var3: 1
    var4: 1
***/

% Regression test for bug #312.
% The element constraint below was being incorrectly flattened to the FlatZinc
% array_int_element constraint, rather than array_var_int_element - the former
% is a type error.


include "globals.mzn";

var int: var1;
var int: var2;
var int: var3;
var int: var4;

constraint (var1 = 1);
constraint (var2 = 2);

constraint element(var4,[var1, var2],var3);

solve minimize var3;
output ["var1:", show(var1),"\n"];
