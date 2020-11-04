/***
!Test
expected:
- !Result
  solution: !Solution
    x: 4
    r: true
***/
predicate no_mixed(var int: x);
predicate start(var int: x) = no_mixed(x);
predicate start_reif(var int: x, var bool: b) = x + b = 5;

var 1..4: x;
var bool: r;
constraint start(x) <-> r;
