/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [true]
***/

% Previously we would only eval_array_lit on the in part

array[int] of var bool: x :: add_to_output = [y=1 | y in let {
  array [1..1] of var 1..2: z;
} in z];

constraint x[1];
