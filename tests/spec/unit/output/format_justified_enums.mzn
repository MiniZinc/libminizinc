/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: '>      [Hello, World]<'
***/

enum Foo = {Hello, World};
output [">", format(20, [Hello, World]), "<"];
