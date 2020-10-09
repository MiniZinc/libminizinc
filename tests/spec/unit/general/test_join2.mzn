/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: 10, 5.3, A String, foobar
***/

string: s = join(", ", [show(10), show(5.3), "A String", "foo" ++ "bar"]);
solve satisfy;
output [s];
