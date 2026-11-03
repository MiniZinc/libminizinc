/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*let variable `z' must be initialised, since it has par fields.*
***/

% The par field of an uninitialised var record in a let has no value
constraint let { record(var bool: a, int: b): z } in z.b = 3;
