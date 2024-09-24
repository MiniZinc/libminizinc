/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc most_specific_reif.fzn
***/

predicate my_pred(var int: x, var int: y);
predicate my_pred_reif(var int: x, var int: y, var bool: r);
predicate my_pred_imp(var int: x, var int: y, var bool: r);
predicate my_pred_reif(var int: x, int: y, var bool: r);

var 1..10: x;
var bool: b;
constraint b \/ my_pred(x, 1);
constraint b \/ my_pred(1, x);
constraint b xor my_pred(2, x);