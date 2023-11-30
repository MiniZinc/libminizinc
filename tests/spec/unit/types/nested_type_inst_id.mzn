/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*type-inst variable \$X used in both array and non-array position.*
***/

% TODO: Maybe this should actually be allowed since we have tuples now.
function tuple(array [$$E] of $X): foo(tuple(array [$X] of $$E));
