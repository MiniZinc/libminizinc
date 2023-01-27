/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    eq1: true
    eq0: false
    nq1: true
    nq0: false
    in1: true
    in0: false
    le1: true
    le0: false
    lq1: true
    lq0: false
    merge1: {x: 1, y: 2.0, z: true}
***/

bool: eq1 ::output = (A: 1, B: "Albus") = (A: 1, B: "Albus");
bool: eq0 ::output = (C: 1, D: 2.3) = (C: 1, D: 2.4);

bool: nq1 ::output = (F: "Albus", E: 1) != (E: 100, F: "Audrey");
bool: nq0 ::output = (E: 100,F: "Audrey") != (E: 100, F: "Audrey");

bool: in1 ::output = (A: 1, B: false) in [
  (B:true, A: 0),
  (B:false, A: 0),
  (B:true, A: 1),
  (B:false, A: 1),
];
bool: in0 ::output = (A: 0.1, B: 23) in [
  (A: 1.0, B: 23),
  (A: 0.0, B: 23),
  (A: 0.1, B: 10),
  (A: 0.1, B: 100),
];

bool: le1 ::output = (H: 0, G: true) < (G: true, H: 1);
bool: le0 ::output = (G: false, H: 1.0) < (G: false, H: 1.0);

bool: lq1 ::output = (A: false, G: 1.0) <= (A: false, G: 1.0);
bool: lq0 ::output = (A: "Audrey", G: 1) <= (A: "Audrey", G: 0);

bool: ge1 ::output = (C: 1, D: false) > (C: 0, D: true);
bool: ge0 ::output = (C: 0, E: false, D: 0.1) > (E: false, D: 0.1, C: 0,);

bool: gq1 ::output = (A: 1, B: false) >= (A: 1, B: false);
bool: gq0 ::output = (A: 0, B: 0.1, C: false) > (A: 0, B: 0.1, C: true);

record(bool: z) ++ record(int: x) ++ record(float: y): merge1 ::output = (x: 1, z: true) ++ (y: 2.0);