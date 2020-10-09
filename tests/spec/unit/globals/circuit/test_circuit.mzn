/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    c1: [3, 4, 2, 1]
    c2: [4, 5, 3, 2]
  - !Solution
    c1: [2, 3, 4, 1]
    c2: [4, 5, 3, 2]
  - !Solution
    c1: [4, 3, 1, 2]
    c2: [4, 5, 3, 2]
  - !Solution
    c1: [3, 1, 4, 2]
    c2: [4, 5, 3, 2]
  - !Solution
    c1: [2, 4, 1, 3]
    c2: [4, 5, 3, 2]
  - !Solution
    c1: [4, 1, 2, 3]
    c2: [4, 5, 3, 2]
  - !Solution
    c1: [3, 4, 2, 1]
    c2: [3, 4, 5, 2]
  - !Solution
    c1: [2, 3, 4, 1]
    c2: [3, 4, 5, 2]
  - !Solution
    c1: [4, 3, 1, 2]
    c2: [3, 4, 5, 2]
  - !Solution
    c1: [3, 1, 4, 2]
    c2: [3, 4, 5, 2]
  - !Solution
    c1: [2, 4, 1, 3]
    c2: [3, 4, 5, 2]
  - !Solution
    c1: [4, 1, 2, 3]
    c2: [3, 4, 5, 2]
  - !Solution
    c1: [3, 4, 2, 1]
    c2: [5, 4, 2, 3]
  - !Solution
    c1: [2, 3, 4, 1]
    c2: [5, 4, 2, 3]
  - !Solution
    c1: [4, 3, 1, 2]
    c2: [5, 4, 2, 3]
  - !Solution
    c1: [3, 1, 4, 2]
    c2: [5, 4, 2, 3]
  - !Solution
    c1: [2, 4, 1, 3]
    c2: [5, 4, 2, 3]
  - !Solution
    c1: [4, 1, 2, 3]
    c2: [5, 4, 2, 3]
  - !Solution
    c1: [3, 4, 2, 1]
    c2: [4, 2, 5, 3]
  - !Solution
    c1: [2, 3, 4, 1]
    c2: [4, 2, 5, 3]
  - !Solution
    c1: [4, 3, 1, 2]
    c2: [4, 2, 5, 3]
  - !Solution
    c1: [3, 1, 4, 2]
    c2: [4, 2, 5, 3]
  - !Solution
    c1: [2, 4, 1, 3]
    c2: [4, 2, 5, 3]
  - !Solution
    c1: [4, 1, 2, 3]
    c2: [4, 2, 5, 3]
  - !Solution
    c1: [3, 4, 2, 1]
    c2: [3, 5, 2, 4]
  - !Solution
    c1: [2, 3, 4, 1]
    c2: [3, 5, 2, 4]
  - !Solution
    c1: [4, 3, 1, 2]
    c2: [3, 5, 2, 4]
  - !Solution
    c1: [3, 1, 4, 2]
    c2: [3, 5, 2, 4]
  - !Solution
    c1: [2, 4, 1, 3]
    c2: [3, 5, 2, 4]
  - !Solution
    c1: [4, 1, 2, 3]
    c2: [3, 5, 2, 4]
  - !Solution
    c1: [3, 4, 2, 1]
    c2: [5, 2, 3, 4]
  - !Solution
    c1: [2, 3, 4, 1]
    c2: [5, 2, 3, 4]
  - !Solution
    c1: [4, 3, 1, 2]
    c2: [5, 2, 3, 4]
  - !Solution
    c1: [3, 1, 4, 2]
    c2: [5, 2, 3, 4]
  - !Solution
    c1: [2, 4, 1, 3]
    c2: [5, 2, 3, 4]
  - !Solution
    c1: [4, 1, 2, 3]
    c2: [5, 2, 3, 4]
options:
  all_solutions: true
***/

include "circuit.mzn";

array[1..4] of var 1..4: c1;
array[2..5] of var -100..100: c2;

constraint circuit(c1);
constraint circuit(c2);

solve satisfy;

output [
   "c1 = array1d(1..4, ", show(c1), ");\n",
   "c2 = array1d(2..5, ", show(c2), ");\n"
];
