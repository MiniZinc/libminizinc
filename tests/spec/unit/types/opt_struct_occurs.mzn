/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    o1: true
    o2: false
    a1: false
    a2: true
    d: [1, 2]
    dr: {"x": 5}
***/

% `occurs'/`absent'/`deopt' treat an optional struct as a single optional value:
% `absent(y)' holds exactly when `y = <>'.
opt tuple(int, int): a = (1, 2);
opt tuple(int, int): b = <>;
opt record(int: x): r = (x: 5);

bool: o1 :: output = occurs(a);
bool: o2 :: output = occurs(b);
bool: a1 :: output = absent(a);
bool: a2 :: output = absent(b);
tuple(int, int): d :: output = deopt(a);
record(int: x): dr :: output = deopt(r);
