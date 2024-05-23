/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

constraint assert(string_length("a b\t") = 4, "Incorrect");
% 4 bytes, but one code point
constraint assert(string_length("😀") = 1, "Incorrect");
