/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution
    _output_item: '[1][true]'
- !Result
  solution: !Solution
    _output_item: '[2][false]'
***/

array[1..1] of var 1..2: x;
predicate foo() = x[1] = 1;
array [1..1] of var bool: y = [foo()];
output [show(x), show(y)];
