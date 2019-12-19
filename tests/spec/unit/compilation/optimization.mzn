/***
--- !Test
type: compile
solvers: [gecode]
options: 
  -O0: true
expected: !FlatZinc optimization_0.fzn
--- !Test
type: compile
solvers: [gecode]
options: 
  -O1: true
expected: !FlatZinc optimization_1.fzn
--- !Test
type: compile
solvers: [gecode]
options: 
  -O2: true
expected: !FlatZinc optimization_2.fzn
--- !Test
type: compile
solvers: [gecode]
options: 
  -O3: true
expected: !FlatZinc optimization_3.fzn
***/

var 1..10: x;
var set of 1..10: y;

constraint forall (i in y) (i mod 2 = 1);
constraint x >= 5 <-> x in y;