/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    objective: 9
    _output_item: 'x = 10'
***/

var 1..10: objective;

constraint let {
  var 1..10: x;
  constraint x > objective;
} in output_to_section("default", "x = \(x)");

solve maximize objective;

output [];
