/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    makespan: 11
--- !Test
solvers: [gecode, chuffed]
options:
  -G: std
expected: !Result
  solution: !Solution
    makespan: 11
***/

include "cumulatives.mzn";

set of int: TIME = 1..20;
enum TASK = T(1..5);
enum MACHINE = M(1..2);

array [TASK, MACHINE] of TIME: d =
  [|          M(1): M(2):
   | T(1):    5,    7,
   | T(2):    3,    3,
   | T(3):    4,    3,
   | T(4):    9,    7,
   | T(5):    3,    5,
   |];

array [TASK, MACHINE] of TIME: r =
  [|          M(1): M(2):
   | T(1):    2,    7,
   | T(2):    6,    4,
   | T(3):    4,    2,
   | T(4):    5,    6,
   | T(5):    2,    1,
   |];

array [MACHINE] of int: bound = [M(1): 6, M(2): 6];

array [TASK] of var TIME: start;
array [TASK] of var TIME: duration :: output = [t: d[t, machine[t]] | t in TASK];
array [TASK] of var int: resource :: output = [t: r[t, machine[t]] | t in TASK];
array [TASK] of var MACHINE: machine;

array [MACHINE, TIME] of int: used :: output_only :: output = [
  (m, t): sum (
    i in TASK where
      fix(start[i]) <= t /\
      fix(start[i]) + fix(duration[i]) > t /\
      fix(machine[i]) = m
  ) (
    fix(resource[i])
  )
|
  m in MACHINE,
  t in TIME
];

constraint cumulatives(
  start,
  duration,
  resource,
  machine,
  bound,
  true
);

var int: makespan :: output = max (t in TASK) (start[t] + duration[t]);
solve minimize makespan;

