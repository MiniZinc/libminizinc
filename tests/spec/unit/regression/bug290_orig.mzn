/***
!Test
expected:
- !Result
  solution: !Solution
    b: true
***/

% Regression test for bug #290: this model caused self-assignments to
% be created for introduced variables.

% Dummy variable, for output only.
var bool: b :: add_to_output = true;

set of int: R = 1..2;

array[R,R] of int: a = array2d(R, R, set2array(1..card(R)*card(R)));

array[R,R] of var 0..1: x;

array[R] of var int: z = [ sum([a[s,t] * x[s,t] | t in R]) | s in R ]; 
constraint forall([ z[s] = sum([a[s,t] * x[s,t] | t in R]) | s in R ]);

solve satisfy;

output ["b = ", show(b), "\n"];
