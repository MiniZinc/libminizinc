/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: !!set {1, 2}
    b: !!set {1, 2, 3}
    c: !!set {4, 5}
    d: 2
    e: !!set {1, 2}
***/

% par int cardinality: card(a) == 2
var set(2) of 1..5: a ::output;
constraint 1 in a /\ 2 in a;

% par int via assignment
int: k = 3;
var set(k) of 1..5: b ::output;
constraint 1 in b /\ 2 in b /\ 3 in b;

% var int cardinality: card(c) == d  (d is determined by the assignment below)
var 1..5: d ::output;
constraint d = 2;
var set(d) of 1..5: c ::output;
constraint 4 in c /\ 5 in c;

% let-bound var set with var int cardinality
var set of 1..5: e ::output = let { var 1..3: n; constraint n = 2; var set(n) of 1..5: s; constraint 1 in s /\ 2 in s; } in s;

solve satisfy;
