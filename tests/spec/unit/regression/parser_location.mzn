/***
---
!Test
solvers: [gecode]
expected: !Error
  regex: .*parser_location\.mzn:8.*
***/
set of 1..1: x = 1; % Trigger an error at this var decl
