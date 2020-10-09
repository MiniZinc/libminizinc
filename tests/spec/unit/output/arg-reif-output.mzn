/***
!Test
expected: !Error
  type: MiniZincError
***/

test test1(1..10: x) = true;
solve satisfy;
output [ if test1(11) then "DID NOT WORK" else "WORKED" endif ];
