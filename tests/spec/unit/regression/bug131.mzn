/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: false
    x: 1
    y: 1
  - !Solution
    b: false
    x: 2
    y: 2
  - !Solution
    b: false
    x: 3
    y: 3
  - !Solution
    b: true
    x: 2
    y: 1
  - !Solution
    b: true
    x: 3
    y: 1
  - !Solution
    b: true
    x: 1
    y: 2
  - !Solution
    b: true
    x: 3
    y: 2
  - !Solution
    b: true
    x: 1
    y: 3
  - !Solution
    b: true
    x: 2
    y: 3
options:
  all_solutions: true
***/

% Regression test for bug #131. 

var 1..3: x;
var 1..3: y;
var bool: b;

predicate foo(array[int] of var int: x);

predicate foo_reif(array[int] of var int: x, var bool: r) =
    r <-> forall(i,j in index_set(x) where i < j) ( x[i] != x[j] );

constraint foo([x,y]) <-> b;

solve satisfy;

output [
    "b = ", show(b), ";\n",
    "x = ", show(x), ";\n",
    "y = ", show(y), ";\n"
];
