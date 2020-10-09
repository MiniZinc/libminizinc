/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: foosbarsbazs
***/

array[1..3] of string: something = ["foo", "bar", "baz"];
array[1..3] of string: x = [s ++ "s" | s in something];
solve satisfy;
output x;
