/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: "A?"
***/

enum Foo = {'A?'};
Foo: x = 'A?';

output [show(x)];
