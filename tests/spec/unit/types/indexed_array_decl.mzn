/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a:
    - 10
    - 20
    - 30
    - 40
    b:
    - - 2
      - 3
      - 4
    - - 3
      - 4
      - 5
    - - 4
      - 5
      - 6
***/

int: n = 4;
function int: f(int: c) = c * 10;

% 1D: per-element upper bound depends on index
array[c in 1..n] of var 1..f(c): a ::output;
constraint forall(c in 1..n)(a[c] = f(c));

% Multi-dim
function int: g(int: i, int: j) = i + j;
array[i in 1..3, j in 1..3] of var 1..g(i,j): b ::output;
constraint forall(i, j in 1..3)(b[i,j] = g(i,j));

solve satisfy;
