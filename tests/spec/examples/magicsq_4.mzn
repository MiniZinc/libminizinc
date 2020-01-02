/***
!Test
solvers: [gecode, chuffed] # Too slow on cbc
expected:
- !Result
  solution: !Solution
    a:
    - [16, 1, 15, 2]
    - [5, 8, 10, 11]
    - [4, 13, 3, 14]
    - [9, 12, 6, 7]
- !Result
  solution: !Solution
    a:
    - [12, 1, 14, 7]
    - [15, 4, 9, 6]
    - [2, 13, 8, 11]
    - [5, 16, 3, 10]
- !Result
  solution: !Solution
    a:
    - [10, 5, 11, 8]
    - [6, 9, 7, 12]
    - [3, 4, 14, 13]
    - [15, 16, 2, 1]
- !Result
  solution: !Solution
    a:
    - [10, 11, 5, 8]
    - [6, 7, 9, 12]
    - [3, 2, 16, 13]
    - [15, 14, 4, 1]
- !Result
  solution: !Solution
    a:
    - [16, 2, 9, 7]
    - [3, 13, 6, 12]
    - [5, 11, 4, 14]
    - [10, 8, 15, 1]
- !Result
  solution: !Solution
    a:
    - [8, 11, 6, 9]
    - [13, 7, 10, 4]
    - [12, 14, 3, 5]
    - [1, 2, 15, 16]
***/

include "magicsq.mzn.model";

n = 4;
