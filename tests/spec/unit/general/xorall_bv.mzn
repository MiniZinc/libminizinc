/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: Ok
***/

constraint xorall([]) = false;
constraint xorall([false]) = false;
constraint xorall([true]) = true;
constraint xorall([false, true]) = true;
constraint xorall([true, false]) = true;
constraint xorall([true, true]) = false;
constraint xorall([false, false]) = false;

solve satisfy;
output ["Ok"];
