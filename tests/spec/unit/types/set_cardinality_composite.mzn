/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    t: !!set {1, 2}
    r1:
      a: !!set {1, 3}
    r2:
      a: !!set {1}
      b: !!set {2, 3}
    nested:
      x: !!set {1}
      y:
        - 7
        - !!set {2, 3}
    arr_of_rec:
    - a: !!set {1}
      b: 9
    - a: !!set {2}
      b: 9
    rec_of_arr:
      a:
        - !!set {1}
        - !!set {2}
***/

% Single-leaf tuple inside a record
var record(var set(2..2) of 1..5: a): r1 ::output;
constraint 1 in r1.a /\ 3 in r1.a;

% Tuple leaf reached via .2
tuple(var set(2..2) of 1..5): tup1;
constraint tup1.1 = {1, 2};
% Project to verify the cardinality constraint was generated on the leaf
var set of 1..5: t ::output = tup1.1;

% Record with two set leaves, different cardinalities
var record(var set(1..1) of 1..5: a, var set(2..2) of 1..7: b): r2 ::output;
constraint 1 in r2.a /\ 2 in r2.b /\ 3 in r2.b;

% Record containing a tuple containing a set leaf (mixed nesting)
var record(var set(1..1) of 1..5: x, tuple(int, var set(2..2) of 1..7): y): nested ::output;
constraint 1 in nested.x;
constraint nested.y.1 = 7;
constraint 2 in nested.y.2 /\ 3 in nested.y.2;

% Array of record-with-set
array[1..2] of var record(var set(1..1) of 1..5: a, int: b): arr_of_rec ::output;
constraint 1 in arr_of_rec[1].a /\ 2 in arr_of_rec[2].a;
constraint arr_of_rec[1].b = 9 /\ arr_of_rec[2].b = 9;

% Record-of-array-of-set (the record itself is par; its fields are var sets)
record(array[1..2] of var set(1..1) of 1..5: a): rec_of_arr ::output;
constraint 1 in rec_of_arr.a[1] /\ 2 in rec_of_arr.a[2];

solve satisfy;
