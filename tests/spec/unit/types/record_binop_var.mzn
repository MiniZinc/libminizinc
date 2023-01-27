/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    eqA: {"a": 2, "b": true}
    nq: {"a": 3, "b": true}
    leA: {"x": 2, "y": 4}
    lqA: {"x": 2, "y": 4}
    arrin: {"a": 3, "b": 5.0}
    merge1: {"a": 2, "b": true, "x": 2, "y": 4}
***/

var record(int: a, bool: b): eqA;
var record(1..2: a, bool: b): eqB;

constraint eqA = eqB;
constraint eqB.b <-> (eqB.a mod 2 = 0);
constraint eqB.a > 1;

var record(2..3: a, bool: b): nq;
constraint nq != eqA;
constraint nq.b = eqA.b;

var record(2..3: x, 4..5: y): leA;
constraint leA < (y: 5, x: 2);

var record(2..3: x, 4..5: y): lqA;
constraint lqA <= (x: 2, y: 4);

var record(3..5: a, float: b): arrin;
constraint arrin in [
  (a: 1, b: 3.5),
  (a: 2, b: 1.0),
  (a: 3, b: 5.0),
  (a: 6, b: 0.1),
];

var record(int: a, bool: b) ++ var record(2..3: x, 4..5: y): merge1 ::output = eqA ++ lqA;