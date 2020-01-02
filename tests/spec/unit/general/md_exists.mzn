/***
--- !Test
options:
  all_solutions: true
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: true
    bs: [[[true, true], [false, false]], [[false, false], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [false, false]], [[false, true], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [false, false]], [[true, false], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [false, false]], [[true, true], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [false, true]], [[false, false], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [false, true]], [[false, true], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [false, true]], [[true, false], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [false, true]], [[true, true], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [true, false]], [[false, false], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [true, false]], [[false, true], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [true, false]], [[true, false], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [true, false]], [[true, true], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [true, true]], [[false, false], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [true, true]], [[false, true], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [true, true]], [[true, false], [false, false]]]
  - !Solution
    b: true
    bs: [[[true, true], [true, true]], [[true, true], [false, false]]]
--- !Test
solvers: [cbc]
expected: !Result
  solution: !Solution
    b: true
    bs: [[[true, true], [false, false]], [[false, false], [false, false]]]
***/

bool: b :: add_to_output = exists([|false, true | true /\false, not true|]);

array[1..2,1..2,1..2] of var bool: bs :: add_to_output;
constraint exists(bs);

constraint bs[1, 1, 1] = true;
constraint bs[1, 1, 2] = true;
constraint bs[2, 2, 1] = false;
constraint bs[2, 2, 2] = false;
