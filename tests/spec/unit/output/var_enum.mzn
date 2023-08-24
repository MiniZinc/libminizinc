/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: '["X_INTRODUCED_17_", "X_INTRODUCED_18_", "X_INTRODUCED_19_"] [X_INTRODUCED_17_, X_INTRODUCED_18_, X_INTRODUCED_19_] [Inch, Inch, Inch]'
***/

enum Unit = {Inch, Meter, Foot};
array[1..3] of var Unit: a;

array[1..3] of string: m = [show(a[i]) | i in 1..3];
string: n = show(a);

output ["\(m) ", n, " ", show(a)];

solve minimize max(a);
