/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    xs: [2, 3]
***/

array[int] of int: x = [1,2,3,4];

int: i = 2;
array[int] of int: xs :: add_to_output = x[i..i+1];
