/***
--- !Test
expected: !FlatZinc bug_629.fzn
options:
  -O1: true
solvers: [cbc]
type: compile
--- !Test
expected: !FlatZinc bug_629.fzn
options:
  -O2: true
solvers: [cbc]
type: compile
***/

% Bug related to mishandling of _globalDir path when -O2 is used

var bool: b;
var 0..1: x = if b then 1 else 0 endif;
