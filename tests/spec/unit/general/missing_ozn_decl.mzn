/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution
    I1: 7
    B2: true
    B3: true
***/

var int: I1 ::add_to_output;
var bool: B2 ::add_to_output;
var bool: B3 ::add_to_output;

constraint ((I1 = 7) <-> B2);
constraint ((I1 = 7) <-> B3);

constraint B3;
