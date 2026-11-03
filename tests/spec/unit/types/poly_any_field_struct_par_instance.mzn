/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    a: 2
***/

% The declared parameter type of g and h is var because of the `any $T' field. Specialising them
% for a par argument (the par version created for output) kept that var on the tuple type,
% which then had no var fields (assertion failure in registerTupleType).
function any $T: g(array [int] of tuple(int, any $T): x) = x[1].2;
function any $T: h(tuple(int, any $T): x) = x.2;
var 1..3: a :: add_to_output;
any: y = g([(1, a)]);
any: z = h((1, (a, true)));
constraint y = 2;
constraint z.2;
