/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: [[[1]]]
    'x@14.5-28|a@13.6': [[[1]]]
    _output_item: 'predicate: [1]'
***/

enum Foo = {A};
array [Foo, Foo, Foo] of var int: foo() = [(i, j, k): let {var 1..3: a} in a | i, j, k in Foo]; 
any: a :: output = let {
    any: x :: output = foo();
    constraint output_to_section("default", "predicate: \(x)");
} in x;

solve minimize sum(a);

output [];
