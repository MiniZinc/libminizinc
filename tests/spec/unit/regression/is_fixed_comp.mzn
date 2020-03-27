/***
!Test
expected:
- !Result
  solution: !Solution
    len: 1
***/

array[1..2] of var bool: x = [true, _];

var int: len ::add_to_output;
constraint len = let {
  array[int] of var bool: x_fixed = [x[i] | i in index_set(x) where is_fixed(x[i])];
} in length(x_fixed);
