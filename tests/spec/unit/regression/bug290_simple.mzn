/***
!Test
expected:
- !Result
  solution: !Solution
    b: true
***/

% Simplified regression test for bug #290: this model caused a self-assignment
% to be created for one of the introduced variables.

% Dummy variable, for output only.
var bool: b :: add_to_output = true;

array[1..2,1..2] of int: a = array2d(1..2, 1..2, [1, 2, 3, 4]);
array[1..2,1..2] of var 0..1: x;
array[1..2] of var int: z = [ sum([a[s,t] * x[s,t] | t in 1..2]) | s in 1..2 ]; 
constraint z[1] = sum([a[1,t] * x[1,t] | t in 1..2]);

solve satisfy;

output ["b = ", show(b), "\n"];
