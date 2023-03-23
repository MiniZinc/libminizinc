/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: '{C}, [(A,)]'
***/

enum Foo = {A, B};
enum Bar = {C};
function array [$$E] of tuple($T): foo($T: x, $$E: y) = [y: (x,)];
array [C..C] of tuple(Foo): x = foo(A, C);

output ["\(index_set(x)), \(x)"];
