/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected:
- !Result
  solution:
  - !Solution
    b: true
    bs: [[[true, true], [true, true]], [[true, true], [true, true]]]
---
!Test
solvers: [cbc, chuffed]
expected: !Result
  solution: !Solution
    b: true
    bs: [[[true, true], [true, true]], [[true, true], [true, true]]]
***/

bool: b :: add_to_output = forall([|true, true | true \/ false, not false|]);
array[1..2,1..2,1..2] of var bool: bs :: add_to_output;
constraint forall(bs);

solve satisfy;
