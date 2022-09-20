/***
!Test
solver: [gecode]
extra_files:
- json_input_multidim_enum_str.json
expected: !Result
  solution: !Solution
    x: [[A, B], [C, D]]
***/

enum Foo = {A, B, C, D};
array [int, int] of Foo: x :: output;
