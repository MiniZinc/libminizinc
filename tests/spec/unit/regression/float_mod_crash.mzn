/***
!Test
expected: !Error
  type: MiniZincTypeError
***/

% Regression test for wrong definition of float mod in stdlib causing crash

float: x :: add_to_output = 2.0 mod 1.0;
