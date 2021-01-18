/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 1
***/

int: 'x' :: add_to_output = 1;

constraint x = 1;
