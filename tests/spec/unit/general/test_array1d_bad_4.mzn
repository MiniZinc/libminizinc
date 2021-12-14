/***
!Test
expected: !Error
  type: TypeError
***/

array [1..3] of var int: a = [1, 2, 3];
array [11..13] of var int: b = a;

solve satisfy;
output [show([a,b])];