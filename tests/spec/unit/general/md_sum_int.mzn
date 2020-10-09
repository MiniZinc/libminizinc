/***
--- !Test
options:
  all_solutions: true
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    a: 55
    b: 10
  - !Solution
    a: 55
    b: 11
  - !Solution
    a: 55
    b: 12
  - !Solution
    a: 55
    b: 13
  - !Solution
    a: 55
    b: 14
  - !Solution
    a: 55
    b: 15
  - !Solution
    a: 55
    b: 16
  - !Solution
    a: 55
    b: 17
  - !Solution
    a: 55
    b: 18
  - !Solution
    a: 55
    b: 19
  - !Solution
    a: 55
    b: 20
  - !Solution
    a: 55
    b: 21
  - !Solution
    a: 55
    b: 22
  - !Solution
    a: 55
    b: 23
  - !Solution
    a: 55
    b: 24
  - !Solution
    a: 55
    b: 25
  - !Solution
    a: 55
    b: 26
  - !Solution
    a: 55
    b: 27
  - !Solution
    a: 55
    b: 28
  - !Solution
    a: 55
    b: 29
  - !Solution
    a: 55
    b: 30
  - !Solution
    a: 55
    b: 31
  - !Solution
    a: 55
    b: 32
  - !Solution
    a: 55
    b: 33
  - !Solution
    a: 55
    b: 34
  - !Solution
    a: 55
    b: 35
  - !Solution
    a: 55
    b: 36
  - !Solution
    a: 55
    b: 37
  - !Solution
    a: 55
    b: 38
  - !Solution
    a: 55
    b: 39
  - !Solution
    a: 55
    b: 40
  - !Solution
    a: 55
    b: 4
  - !Solution
    a: 55
    b: 5
  - !Solution
    a: 55
    b: 6
  - !Solution
    a: 55
    b: 7
  - !Solution
    a: 55
    b: 8
  - !Solution
    a: 55
    b: 9

***/

int: a :: add_to_output = sum([|1, 2, 3, 4, 5 | 6, 7, 8, 9, 10|]);

array[1..2,1..2] of var 1..10: vs;
var -100..100: b :: add_to_output = sum(vs);

solve satisfy;

output [
  "% a = ", show(a), ";\n",
  "b = ", show(b), ";\n"
];
