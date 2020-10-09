/***
!Test
expected:
- !Result
  solution: !Solution
    a:
    - [8, 1, 6]
    - [3, 5, 7]
    - [4, 9, 2]
- !Result
  solution: !Solution
    a:
    - [6, 7, 2]
    - [1, 5, 9]
    - [8, 3, 4]
- !Result
  solution: !Solution
    a:
    - [2, 7, 6]
    - [9, 5, 1]
    - [4, 3, 8]
- !Result
  solution: !Solution
    a:
    - [4, 3, 8]
    - [9, 5, 1]
    - [2, 7, 6]
- !Result
  solution: !Solution
    a:
    - [8, 3, 4]
    - [1, 5, 9]
    - [6, 7, 2]
- !Result
  solution: !Solution
    a:
    - [2, 9, 4]
    - [7, 5, 3]
    - [6, 1, 8]
***/

include "magicsq.mzn.model";

n = 3;
