/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: !!set {1, 3}
    b:
    - !!set {2}
    - !!set {3}
    c: 3
    p: !!set {1, 2}
    q: !!set {4, 5}
***/

% toplevel var set with a literal cardinality bound
var set(2..2) of 1..5: a ::output;
constraint 1 in a /\ 3 in a;

% array of var set: the cardinality constraint applies element-wise
array[1..2] of var set(1..1) of 1..3: b ::output;
constraint 2 in b[1];
constraint 3 in b[2];

% let-bound var set
var int: c ::output = let { var set(3..3) of 1..3: s; } in card(s);

% par set with a cardinality and a right-hand side
set(2..2) of 1..10: p ::output = {1, 2};

% cardinality given by a set expression
set of int: ecard = {2};
var set(ecard) of 1..5: q ::output;
constraint 4 in q /\ 5 in q;

solve satisfy;
