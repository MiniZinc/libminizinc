/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    sts: [{"x": 0}, {"x": 1}]
***/
type State = record(0..1: x);

predicate trans_closure(array[1..2] of var State: sts) = (sts[1].x + 1 == sts[2].x);

array[1..2] of var State: sts;
constraint trans_closure(sts);
