/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: !!set {}
    ubx: !!set {a,b,c}
    y: a
    lby: a
    uby: c
    domy: !!set {a,b,c}
***/
enum X = {a, b, c};

var set of X: x;
set of X: ubx ::output = ub(x);
constraint card(x) < 1;

var X: y;
X: lby ::output = lb(y);
X: uby ::output = ub(y);
set of X: domy ::output = dom(y);
constraint y < b;
