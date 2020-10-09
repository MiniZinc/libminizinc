/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: "105.31..3A String"
***/

string: s = concat([show(10), show(5.3), show({1, 2, 3}), "A String"]);
solve satisfy;
output [s];
