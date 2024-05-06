/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: '{A, C}, <>, [{A, B}, <>, {A, C}, <>]'
***/

enum Foo = {A, B, C};
opt set of Foo: x = {A, C};
opt set of Foo: y = <>;
array [int] of opt set of Foo: z = [{A, B}, <>, x, y];

output ["\(x), \(y), \(z)"];
