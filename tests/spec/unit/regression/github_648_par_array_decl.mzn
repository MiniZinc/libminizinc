/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*declared domain of `x\[1\]' is 1\.\.3, but assigned value is 0.*
***/

% Previously we included monomorphised domain in undefined result message

function set of int: foo(any $T) = 1..3;
array [1..1] of foo("a"): x = [0];
