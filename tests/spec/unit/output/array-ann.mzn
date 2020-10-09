/***
!Test
expected: !Result
  solution: !Solution
    _output_item: Ok
***/

% Check that arrays of annotation varibles (and assignments to such) do
% not get emitted in .ozn files.

array[1..3] of ann: foo;
solve satisfy;
output ["Ok"];

foo = [first_fail, domain, bounds];
