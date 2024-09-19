/***
!Test
extra_files:
- enum_int_coercion.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    t: [[1,2,3]]
***/
enum E = {A};
array[int, int] of int: t ::output;
