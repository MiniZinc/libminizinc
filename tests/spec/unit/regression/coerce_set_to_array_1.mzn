/***
!Test
expected: !Result
  solution: !Solution
    x: 1.0
***/

% Used to give an error due to wrong coercion result when array2set is combined with int2float

function float: foo(array [int] of float: x) = x[1];
float: x :: add_to_output = foo({1});
