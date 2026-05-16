/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    s:
    - !!set {1, 2}
    - !!set {1, 2, 3}
    - !!set {1, 2, 3, 4}
***/

% Indexed array declaration composed with set(e) cardinality marker on the
% element type.  Per-element cardinality grows with the index.
int: n = 3;
function int: f(int: c) = c + 1;
array[c in 1..n] of var set(f(c)) of 1..10: s ::output;
constraint forall(c in 1..n)(forall(v in 1..f(c))(v in s[c]));

solve satisfy;
