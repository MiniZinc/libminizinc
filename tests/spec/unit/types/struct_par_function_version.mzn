/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    p: true
    a: false
    b: true
    c: false
***/

function record(var 1..2: a, var bool: b): bar(tuple(var 2..3, tuple(var bool)): x) = (a: x.1, b: x.2.1);

predicate foo(var int: x, var bool: b) = let {
  any: y = bar((x, (b,)));
} in true;

var bool: p;
constraint p;

any: a :: output_only :: output = fix(foo(1, fix(p)));
any: b :: output_only :: output = fix(foo(2, fix(p)));
any: c :: output_only :: output = fix(foo(3, fix(p)));
