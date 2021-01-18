/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: "Foo~(A?)"
***/

enum 'Foo!' = {'A?'};
enum 'Bar' = 'Foo~'('Foo!') ++ {'B/'};

Bar: x = 'Foo~'('A?');

output [show(x)];
