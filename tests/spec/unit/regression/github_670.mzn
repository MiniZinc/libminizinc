/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/


enum A = { X };
enum B = { Y };

function int: f(array[int] of var B: arr) = 0;

function int: f2(array[int] of var B: arr) = f(reverse(arr));
