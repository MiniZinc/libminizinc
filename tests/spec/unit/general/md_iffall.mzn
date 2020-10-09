/***
--- !Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: false
    bs: [[[false, false], [false, false]], [[false, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, false]], [[false, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, false]], [[false, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, false]], [[false, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, false]], [[true, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, false]], [[true, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, false]], [[true, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, false]], [[true, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, true]], [[false, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, true]], [[false, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, true]], [[false, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, true]], [[false, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, true]], [[true, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, true]], [[true, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, true]], [[true, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [false, true]], [[true, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, false]], [[false, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, false]], [[false, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, false]], [[false, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, false]], [[false, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, false]], [[true, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, false]], [[true, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, false]], [[true, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, false]], [[true, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, true]], [[false, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, true]], [[false, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, true]], [[false, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, true]], [[false, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, true]], [[true, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, true]], [[true, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, true]], [[true, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, false], [true, true]], [[true, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, false]], [[false, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, false]], [[false, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, false]], [[false, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, false]], [[false, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, false]], [[true, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, false]], [[true, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, false]], [[true, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, false]], [[true, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, true]], [[false, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, true]], [[false, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, true]], [[false, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, true]], [[false, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, true]], [[true, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, true]], [[true, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, true]], [[true, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [false, true]], [[true, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, false]], [[false, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, false]], [[false, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, false]], [[false, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, false]], [[false, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, false]], [[true, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, false]], [[true, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, false]], [[true, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, false]], [[true, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, true]], [[false, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, true]], [[false, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, true]], [[false, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, true]], [[false, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, true]], [[true, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, true]], [[true, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, true]], [[true, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[false, true], [true, true]], [[true, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, false]], [[false, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, false]], [[false, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, false]], [[false, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, false]], [[false, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, false]], [[true, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, false]], [[true, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, false]], [[true, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, false]], [[true, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, true]], [[false, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, true]], [[false, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, true]], [[false, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, true]], [[false, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, true]], [[true, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, true]], [[true, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, true]], [[true, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [false, true]], [[true, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, false]], [[false, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, false]], [[false, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, false]], [[false, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, false]], [[false, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, false]], [[true, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, false]], [[true, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, false]], [[true, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, false]], [[true, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, true]], [[false, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, true]], [[false, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, true]], [[false, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, true]], [[false, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, true]], [[true, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, true]], [[true, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, true]], [[true, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, false], [true, true]], [[true, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, false]], [[false, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, false]], [[false, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, false]], [[false, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, false]], [[false, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, false]], [[true, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, false]], [[true, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, false]], [[true, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, false]], [[true, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, true]], [[false, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, true]], [[false, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, true]], [[false, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, true]], [[false, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, true]], [[true, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, true]], [[true, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, true]], [[true, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [false, true]], [[true, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, false]], [[false, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, false]], [[false, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, false]], [[false, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, false]], [[false, true], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, false]], [[true, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, false]], [[true, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, false]], [[true, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, false]], [[true, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, true]], [[false, false], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, true]], [[false, false], [true, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, true]], [[false, true], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, true]], [[false, true], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, true]], [[true, false], [false, true]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, true]], [[true, false], [true, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, true]], [[true, true], [false, false]]]
  - !Solution
    b: false
    bs: [[[true, true], [true, true]], [[true, true], [true, true]]]
--- !Test
solvers: [cbc]
expected: !Result
  solution: !Solution
    b: false
    bs: [[[false, false], [false, false]], [[false, false], [false, false]]]
***/

bool: b :: add_to_output = iffall([|true, false | true /\ false, false|]);

array[1..2,1..2,1..2] of var bool: bs :: add_to_output;
constraint iffall(bs);

solve satisfy;
