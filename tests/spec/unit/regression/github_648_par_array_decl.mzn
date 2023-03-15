/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*parameter value out of range, declared range of array `x' is 1\.\.3, but element at index 1 is 0.*
***/

% Previously we included monomorphised domain in undefined result message

function set of int: foo(any $T) = 1..3;
array [1..1] of foo("a"): x = [0];
