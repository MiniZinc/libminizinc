/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: '1'
***/

% Regression test for GitHub #751 where the par versions of polymorphic functions
% weren't made available for use in output.

function var $$E: foo(var $$E: x) = let {
  var dom(x): a;
  constraint a = x;
} in a;

function $$E: foo($$E: x) = x;

var 1..10: x;
output ["\(foo(x))"];

solve minimize x;
