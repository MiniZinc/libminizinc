/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    b: true
***/
ann: ann_test;
predicate add_ann(var int: x) = annotate(x, ann_test);
predicate get_ann(var int: x, var bool: b) = b <-> has_ann(x, ann_test);

var 1..2: x;
var bool: b;
constraint add_ann(x);
constraint get_ann(x, b);

