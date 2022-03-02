/***
!Test
expected: !Result
  status: SATISFIED
***/

% Used to fail because {} is par set of bottom

enum Foo = {A};
set of Foo: x :: output = to_enum(Foo, {});
array [int] of int: y :: output = [i | i in {}];
