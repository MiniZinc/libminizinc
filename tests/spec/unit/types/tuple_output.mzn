/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: "full var: (0, true, 1.5)\nvar array: [(2, false), (1, true)]\nnested: (false, (3, 4))\nelement: 3\npartial: (10, true)\ndata: (-3.2, false)\nenumtup: ((x: a a, y: c), c)\n"
***/

% simple tuple variable
tuple(var int, var bool, var float): x;
constraint x.1 = 0;
constraint x.2 = true;
constraint x.3 = 1.5;

% array of tuples
array[1..2] of tuple(var 1..2, var bool): y;
constraint forall(i in 1..2) (
  y[i].2 <-> (y[i].1 = 1)
);
constraint y[1].1 > y[2].1;

% nested tuple
tuple(var bool, var tuple(3..4, 3..4)): z;
constraint z.2.1 < (z.2).2;
constraint z.1 <-> (z.2.1 > z.2.2);

% initialised
tuple(int, var bool): init = (10, let {var bool: b} in b);
constraint init.2 <-> init.1 mod 2 == 0;

% data
tuple(float, bool): dat = (-3.2, false);

enum X = {'a a',c};
tuple(record(X:x,X:y),X): enumtup = ((x:'a a',y:c),c);

output ["full var: \(x)\nvar array: \(y)\nnested: \(z)\nelement: \(z.2.1)\npartial: \(init)\ndata: \(dat)\nenumtup: \(enumtup)\n"];

