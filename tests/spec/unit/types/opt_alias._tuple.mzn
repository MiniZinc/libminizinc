/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: null
    y: [1]
***/

% `opt Foo' where Foo is a tuple alias makes the tuple as a whole optional.
type Foo = tuple(int);
opt Foo: x :: output = <>;
opt Foo: y :: output = (1,);
