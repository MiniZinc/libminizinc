/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: "full var: (a: 0, b: true, c: 1.5)\nvar array: [(a: 2, b: false), (a: 1, b: true)]\nnested: (inner: (left: 3, right: 4), outer: false)\nelement: 3\npartial: (x: 10, y: true)\ndata: (e: -3.2, f: false)\n"
***/

% simple record variable
record(var int: a, var bool: b, var float: c): x;
constraint x.a = 0;
constraint x.b = true;
constraint x.c = 1.5;

% array of records
array[1..2] of record(var bool: b, var 1..2: a): y;
constraint forall(i in 1..2) (
  y[i].b <-> (y[i].a = 1)
);
constraint y[1].a > y[2].a;

% nested tuple
record(var bool: outer, var record(3..4: left, 3..4: right): inner): z;
constraint z.inner.left < z.inner.right;
constraint z.outer <-> (z.inner.left > z.inner.right);

% initialised
record(int: x, var bool: y): init = (y: let {var bool: b} in b, x: 10);
constraint init.y <-> init.x mod 2 == 0;

% data
record(float: e, bool: f): dat = (f: false, e: -3.2);

output ["full var: \(x)\nvar array: \(y)\nnested: \(z)\nelement: \(z.inner.left)\npartial: \(init)\ndata: \(dat)\n"];