/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    t:
    - - 10
      - 1
    - - 20
      - 2
    - - 30
      - 3
    aoa:
    - - 11
      - 12
    - - 21
      - 22
    - - 31
      - 32
    r:
    - a: 10
      b: 1
    - a: 20
      b: 2
    - a: 30
      b: 3
***/

int: n = 3;
function int: f(int: c) = c * 10;
function int: h(int: i, int: j) = i * 10 + j;

% Per-element domain on a tuple field
array[c in 1..n] of tuple(var 1..f(c), int): t ::output =
  [(f(c), c) | c in 1..n];

% Per-element domain in array of array, with binders on both levels
array[i in 1..3] of array[j in 1..2] of var 1..h(i,j): aoa ::output;
constraint forall(i in 1..3, j in 1..2)(aoa[i][j] = h(i,j));

% Per-element domain on a record field
array[c in 1..n] of record(var 1..f(c): a, int: b): r ::output =
  [(a: f(c), b: c) | c in 1..n];

solve satisfy;
