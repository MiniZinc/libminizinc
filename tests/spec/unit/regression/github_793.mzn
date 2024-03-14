/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: '[{"e":"A"}] {"v": [{"e":"A"}]}'
***/

enum Foo = { A };

array[int] of Foo: x = [ A ];
record(array[int] of Foo: v): y = (v: x);

output [showJSON(x), " ", showJSON(y)];
