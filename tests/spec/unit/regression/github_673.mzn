/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: '[X, <>]'
***/

enum E = { X };

function array[int] of opt E: f() = [X, <>];

output [show(f())];
