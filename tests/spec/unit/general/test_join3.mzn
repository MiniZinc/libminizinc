/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: 10, 5.3, A String, foobar
***/

array[1..4] of string: v;
string: s = join(", ", v);
v = [show(10), show(5.3), "A String", "foo" ++ "bar"];
solve satisfy;
output [s];
