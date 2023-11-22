/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: 'x = 10'
***/

var 1..10: objective :: foo;

annotation foo(var int: x :: annotated_expression) = let {
  constraint output_to_section("default", "x = \(x)");
} in empty_annotation;

solve maximize objective;

output [];
