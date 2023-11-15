/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: "x = 10\ny = 1\n"
***/

var 1..10: x :: foo;
var 1..5: y;

annotation foo(var int: y :: annotated_expression) = let {
  constraint output_to_section("default", "x = \(y)\n");
} in empty_annotation;

solve maximize x;

output ["y = \(y)\n"];
