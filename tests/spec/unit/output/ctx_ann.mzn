/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    c: 5
***/

predicate my_pred(var int: x) ann: preds =
  let {
    constraint trace("x \(x)\n");
  } in if some in preds then c = 5 else false endif;

var 1..10: c;
annotation some;

constraint my_pred(c) :: some;
