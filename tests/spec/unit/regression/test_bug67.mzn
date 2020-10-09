/***
!Test
expected: !Error
  type: MiniZincError
***/

predicate foo(var int: x); 
var int: x;
var bool: b;
constraint b <-> foo(x);
solve satisfy;