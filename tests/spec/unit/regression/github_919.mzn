/***
!Test
solvers: [gecode]
expected: !Result
- !Solution
  x: 1
***/

% Previously the compiler would generate an invalid par version of foo()
% because the type of t was seen as par instead of containing var members

var 1..2: v;
tuple(var int, int): t = (v, 1);

function var int: bar(var int: x) = 1;
function int: bar(int: x) = 2;

function var int: foo() = bar(t.1);
var int: x :: output = foo();

solve minimize v;
