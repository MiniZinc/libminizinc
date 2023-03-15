/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*parameter value out of range, declared range of `x' is 1\.\.3, but assigned value is 0.*
***/

% Previously we included monomorphised domain in undefined result message

function set of int: foo(any $T) = 1..3;
foo("a"): x = 0;
