/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a:
    - A
    - A
    - B
    - B
***/

enum E = {A, B, C};

% Per-element enum domain: an enum-typed set returned by a function.
function set of E: dom_of(int: c) = if c <= 2 then {A} else {A, B} endif;

array[c in 1..4] of var dom_of(c): a ::output;
constraint a[1] = A;
constraint a[2] = A;
constraint a[3] = B;
constraint a[4] = B;

solve satisfy;
