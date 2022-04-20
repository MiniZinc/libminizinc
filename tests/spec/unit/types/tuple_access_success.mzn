/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: "onetwo"
***/

tuple(int): single = (1,);
tuple(bool, int): double = (false, -1);
tuple(int, bool, float): triple = (1, false, 2.0e2,);

array[1..2] of tuple(int, string): square = [(1, "one"), (-2, "two")];

constraint single.1 == 1;
constraint not double.1;
constraint abs(double.2) == 1;
constraint triple.1 + 1 == 2;
constraint triple.2 \/ true;
constraint 100 <= triple.3 /\ triple.3 <= 300;

constraint square[1].1 == 1;
constraint sum(i in 1..2)(square[i].1) == -1;

output[ square[i].2 | i in 1..2];