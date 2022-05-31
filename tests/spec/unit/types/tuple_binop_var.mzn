/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    eqA: [2, true]
    nq: [3, true]
    leA: [2, 4]
    lqA: [2, 4]
    arrin: [3, 5.0]
***/

var tuple(int, bool): eqA;
var tuple(1..2, bool): eqB;

constraint eqA = eqB;
constraint eqB.2 <-> (eqB.1 mod 2 = 0);
constraint eqB.1 > 1;

var tuple(2..3, bool): nq;
constraint nq != eqA;
constraint nq.2 = eqA.2;

var tuple(2..3, 4..5): leA;
constraint leA < (2, 5);

var tuple(2..3, 4..5): lqA;
constraint lqA <= (2, 4);

var tuple(3..5, float): arrin;
constraint arrin in [
  (1, 3.5),
  (2, 1.0),
  (3, 5.0),
  (6, 0.1),
];