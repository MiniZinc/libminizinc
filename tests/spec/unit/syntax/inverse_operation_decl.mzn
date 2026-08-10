/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution {}
  status: SATISFIED
***/

% An operation may be declared as the inverse of another (parser.yxx:730,744),
% in both the `predicate`/`test` and `function` forms and in both spellings.
predicate p⁻¹(int: x) = x > 0;
test t⁻¹(int: x) = x > 0;
function int: f⁻¹(int: x) = x;
predicate q^-1(int: x) = x > 0;

solve satisfy;
