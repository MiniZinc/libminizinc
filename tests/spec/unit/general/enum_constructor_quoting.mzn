/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: "'Q~'('F!'('\"'))"
***/

enum Foo = { '"' };
enum Bar = 'F!'(Foo);
enum Qux = 'Q~'(Bar);

var Qux: x;

output [showDzn(x)];
