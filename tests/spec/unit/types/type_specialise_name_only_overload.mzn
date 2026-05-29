/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    t1: 10
    t2: 20
    t3: 30
    t4: 40
***/

% Regression test: "name-only" overloads (declarations that differ only in a
% parameter name and are dispatched via named arguments) of a polymorphic
% predicate must be specialised independently, even when they also have opt
% coercion-siblings. Previously this crashed type specialisation
% (assertion in type_specialise.cpp) and, once that was bypassed, silently
% collapsed the siblings to a single instance (wrong results).

predicate pick(array [$$E] of var int: xs, var int: tag, array [$$E] of int: upper_bounds) =
  tag = 10 /\ forall (i in index_set(xs)) (xs[i] <= upper_bounds[i]);
predicate pick(array [$$E] of var int: xs, var int: tag, array [$$E] of int: lower_bounds) =
  tag = 20 /\ forall (i in index_set(xs)) (xs[i] >= lower_bounds[i]);
predicate pick(array [$$E] of var opt int: xs, var int: tag, array [$$E] of int: upper_bounds) =
  tag = 30 /\ forall (i in index_set(xs)) (absent(xs[i]) \/ deopt(xs[i]) <= upper_bounds[i]);
predicate pick(array [$$E] of var opt int: xs, var int: tag, array [$$E] of int: lower_bounds) =
  tag = 40 /\ forall (i in index_set(xs)) (absent(xs[i]) \/ deopt(xs[i]) >= lower_bounds[i]);

array [1..2] of var 0..10: a;
array [1..2] of var opt 0..10: ao;

var int: t1 :: output;
var int: t2 :: output;
var int: t3 :: output;
var int: t4 :: output;

constraint pick(a, t1, upper_bounds: [5, 5]);
constraint pick(a, t2, lower_bounds: [1, 1]);
constraint pick(ao, t3, upper_bounds: [5, 5]);
constraint pick(ao, t4, lower_bounds: [1, 1]);

solve satisfy;
