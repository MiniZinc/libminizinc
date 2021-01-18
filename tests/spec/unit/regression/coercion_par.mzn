/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [1, null]
    y: [1.0, null]
    z: [0.0, null]
***/

% Previously crashed due to using var bool2int/bool2float since par version was missing

array [int] of opt int: x :: add_to_output = [true, <>];
array [int] of opt float: y :: add_to_output = [1, <>];
array [int] of opt float: z :: add_to_output = [false, <>];
