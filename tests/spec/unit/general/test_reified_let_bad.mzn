/***
!Test
expected: !Error
  type: MiniZincError
***/

var 1..10: x;
var bool: b;
constraint b <-> ( let { var 1..10: y } in x = y );
solve satisfy;
