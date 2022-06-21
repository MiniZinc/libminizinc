/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: "onetwo"
***/

record(int: one): single = (one: 1);
record(bool: _a, int: '💀'): double = ('💀': -1, _a: false);
record(int: nr1, bool: nr2, float: nr3): triple = (nr1: 1, nr2: false, nr3: 2.0e2,);

array[1..2] of record(int: left, string: right): square = [(left: 1, right: "one"), (right: "two", left: -2)];

constraint single.one == 1;
constraint not double._a;
constraint abs(double.'💀') == 1;
constraint triple.nr1 + 1 == 2;
constraint triple.n2 \/ true;
constraint 100 <= triple.nr3 /\ triple.nr3 <= 300;

constraint square[1].left == 1;
constraint sum(i in 1..2)(square[i].left) == -1;

output[ square[i].right | i in 1..2];