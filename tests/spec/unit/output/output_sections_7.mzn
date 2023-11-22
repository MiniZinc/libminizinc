/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: "[1]\n"
***/

predicate foo(var int: a) = bar([a]);
predicate bar(array [int] of var int: b) = output_to_json_section("foo", b);
var 1..2: x :: no_output;
constraint foo(x);
output [];
