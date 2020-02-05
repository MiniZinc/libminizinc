/***
!Test
expected: !Error
***/

% Should give an error because sets of floats cannot be coerced to arrays.
% Used to work due to set of float being incorrectly made a subtype of array of float.

function bool: foo(array [int] of float: x) = true;
bool: x :: add_to_output = foo({1.0});
