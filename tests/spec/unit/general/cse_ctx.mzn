/***
!Test
solvers: [gecode]
options:
  -G: std
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: 1
    b: true
    a: true
    c: false
  - !Solution
    x: 0
    b: false
    a: false
    c: true
  - !Solution
    x: 0
    b: false
    a: true
    c: true
  - !Solution
    x: 1
    b: true
    a: false
    c: true
***/

predicate int_eq_imp(var int: a, var int: b, var bool: r);

predicate foo() = a /\ b;

var 0..1: x;

var bool: a;
var bool: b :: output = x = 1;

var bool: c;
constraint c \/ foo();
constraint c xor foo();
