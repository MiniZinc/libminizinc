/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: !Trim |
      [(true, 2), (false, 1)]
      [(a: false, b: 1), (a: true, b: 3)]
      [(false, (2, 10)), (true, (1, 20))]
***/

% fix of an array of tuples or records with var fields must return an array of par tuples or
% records (the instantiated return type was not cv, so it was not made par).
array [1..2] of tuple(var bool, var 1..3): x;
array [1..2] of record(var bool: a, var 1..3: b): r;
array [1..2] of var bool: b;
array [1..2] of var 1..3: v;
array [1..2] of tuple(var bool, tuple(var 1..3, int)): z = [(b[i], (v[i], 10 * i)) | i in 1..2];
function array [int] of tuple(bool, int): g(array [int] of tuple(bool, int): y) = y;
function array [int] of record(bool: a, int: b): h(array [int] of record(bool: a, int: b): y) = y;
function array [int] of tuple(bool, tuple(int, int)): k(array [int] of tuple(bool, tuple(int, int)): y) = y;
constraint x[1].1 /\ x[1].2 = 2 /\ r[2].a /\ r[2].b = 3 /\ z[1].2.1 = 2 /\ z[2].1;
output [show(g(fix(x))), "\n", show(h(fix(r))), "\n", show(k(fix(z))), "\n"];
