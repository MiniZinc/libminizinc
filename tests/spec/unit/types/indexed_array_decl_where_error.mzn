/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

% `where` clauses are not allowed in indexed array index lists.
array[c in 1..3 where c > 1] of var 1..c: x;

solve satisfy;
