/***
!Test
expected:
- !Result
  solution: !Solution
    objective: -2
    x: 2
***/

% Regression test for github bug #8

var 1..2: x;
var int : nx = -x;
solve minimize nx;
