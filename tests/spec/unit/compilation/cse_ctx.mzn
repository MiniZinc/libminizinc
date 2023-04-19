/***
!Test
type: compile
solvers: [gecode]
options:
  newfzn: true
  -G: std
expected: !FlatZinc cse_ctx.fzn
***/

predicate foo() = a /\ b;

var 0..1: x;

var bool: a;
var bool: b :: output = x = 1;

var bool: c;
constraint c \/ foo();
constraint c xor foo();
