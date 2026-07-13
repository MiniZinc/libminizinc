/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: null
    y: {"a": 1}
***/

% `opt Foo' where Foo is a record alias makes the record as a whole optional.
type Foo = record(int: a);
opt Foo: x :: output = <>;
opt Foo: y :: output = (a: 1);
