/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 1
    y: false
***/

% The compiler should really reject this unless the overload
%   function bool: foo(int: i, bool: j)
% exists, but for now we should at least never incorrectly use the version
% that returns int when the original returned bool.

function int: foo(int: i, int: j) = 2;
function var bool: foo(var int: i, bool: j) = false;

var 1..1: x;
var bool: y :: output = foo(x, true);
