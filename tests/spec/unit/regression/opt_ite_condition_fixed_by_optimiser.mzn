/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    n: 3
    y: [1, 2, 3]
    z: 2
***/

% The optional if-then-else expressions below have conditions that only become fixed during
% optimisation, which used to remove the constraints on y and z
predicate p(array [int] of var opt bool: x) = forall (xi in x) (absent(xi) \/ deopt(xi));

var 1..3: n :: add_to_output;
array [1..3] of var 0..5: y :: add_to_output;
var 0..5: z :: add_to_output;
constraint p(s in 1..n) (y[s] = s);
var opt bool: o = if 2 <= n then z = 2 else <> endif;
constraint absent(o) \/ deopt(o);
constraint n = 3;
