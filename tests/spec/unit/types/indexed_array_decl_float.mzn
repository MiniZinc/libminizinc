/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a:
    - 1.5
    - 2.5
    - 3.5
    - 4.5
***/

int: n = 4;
function set of float: dom_of(int: c) = (int2float(c) + 0.5)..(int2float(c) + 0.5);

% Per-element float domain with non-integer values — the leaf must be typed
% as var float for the constraint to be satisfiable.  The old syntactic
% guesser defaulted to BT_INT and turned this into an unsatisfiable int
% problem.
array[c in 1..n] of var dom_of(c): a ::output;
constraint forall(c in 1..n)(a[c] = int2float(c) + 0.5);

solve satisfy;
