/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: 0
    q: true
    b: [false, false, false]
  - !Solution
    x: 1
    q: true
    b: [false, false, false]
  - !Solution
    x: 2
    q: true
    b: [false, false, false]
  - !Solution
    x: 3
    q: false
    b: [false, false, true]
  - !Solution
    x: 3
    q: true
    b: [false, false, true]
  - !Solution
    x: 4
    q: false
    b: [false, true, true]
  - !Solution
    x: 4
    q: true
    b: [false, true, true]
  - !Solution
    x: 5
    q: false
    b: [true, true, true]
  - !Solution
    x: 5
    q: true
    b: [true, true, true]
***/

% The output reads the elements of an array that it cannot recompute from its definition (here
% because of the let), so they are fully reified although they are only used positively.
% Otherwise, for example, x = 5 could be printed with b[1] false.
var 0..5: x;
var bool: q;
array[1..3] of var bool: b :: output = [let { var int: z = x + i } in z > 5 | i in 1..3];
constraint exists(b) \/ q;
