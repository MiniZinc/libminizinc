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
    con1: [1, 2.0, true]
***/

bool: eq1 ::output = (1, "Albus") = (1, "Albus");
bool: eq0 ::output = (1, 2.3) = (1, 2.4);

bool: nq1 ::output = (1, "Albus") != (100, "Audrey");
bool: nq0 ::output = (100, "Audrey") != (100, "Audrey");

bool: in1 ::output = (1, false) in [
  (0, true),
  (0, false),
  (1, true),
  (1, false),
];
bool: in0 ::output = (0.1, 23) in [
  (1.0, 23),
  (0.0, 23),
  (0.1, 10),
  (0.1, 100),
];

bool: le1 ::output = (true, 0) < (true, 1);
bool: le0 ::output = (false, 1.0) < (false, 1.0);

bool: lq1 ::output = (false, 1.0) <= (false, 1.0);
bool: lq0 ::output = ("Audrey", 1) <= ("Audrey", 0);

bool: ge1 ::output = (1, false) > (0, true);
bool: ge0 ::output = (0, 0.1, false) > (0, 0.1, false);

bool: gq1 ::output = (1, false) >= (1, false);
bool: gq0 ::output = (0, 0.1, false) > (0, 0.1, true);

tuple(int, float, bool): con1 ::output = (1, 2.0) ++ (true,);
