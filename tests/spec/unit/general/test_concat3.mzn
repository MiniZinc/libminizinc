/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: Ok
***/

array[1..4] of string: a = ["A", "B", "C", "D"];
string: s = concat([a[4], a[2], a[3], a[1]]);
constraint s = "DBCA";
solve satisfy;
output ["Ok"];
