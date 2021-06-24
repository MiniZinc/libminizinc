/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: "output_item: 10\noutput_to_section: 10\n"
***/

var 1..10: x :: foo;

annotation foo(var int: x :: annotated_expression) = let {
  constraint output_to_section("default", "output_to_section: \(x)\n");
} in empty_annotation;

solve maximize x;

output ["output_item: \(x)\n"];
