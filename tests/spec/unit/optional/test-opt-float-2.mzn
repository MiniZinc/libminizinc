/***
!Test
solvers: [gecode, chuffed, cbc]
expected: !Result
  solution: !Solution
    addn: [0.0, -2.0, -1.0, 0.0, 1.0, 2.0, -2.0, -4.0, -3.0, -2.0, -1.0, 0.0, -1.0, -3.0, -2.0, -1.0, 0.0, 1.0, 0.0, -2.0, -1.0, 0.0, 1.0, 2.0, 1.0, -1.0, 0.0, 1.0, 2.0, 3.0, 2.0, 0.0, 1.0, 2.0, 3.0, 4.0]
    subn: [null, null, null, null, null, null, -2.0, 0.0, -1.0, -2.0, -3.0, -4.0, -1.0, 1.0, 0.0, -1.0, -2.0, -3.0, 0.0, 2.0, 1.0, 0.0, -1.0, -2.0, 1.0, 3.0, 2.0, 1.0, 0.0, -1.0, 2.0, 4.0, 3.0, 2.0, 1.0, 0.0]
    prod: [1.0, -2.0, -1.0, 0.0, 1.0, 2.0, -2.0, 4.0, 2.0, 0.0, -2.0, -4.0, -1.0, 2.0, 1.0, 0.0, -1.0, -2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, -2.0, -1.0, 0.0, 1.0, 2.0, 2.0, -4.0, -2.0, 0.0, 2.0, 4.0]
    divn: [null, null, null, null, null, -2.0, 1.0, 2.0, -2.0, -1.0, -1.0, 0.5, 1.0, -1.0, -0.5, 1.0, -0.5, -1.0, 1.0, 0.5, 2.0, -1.0, -2.0, 2.0, 1.0]
    gt: [true, true, true, true, false, false, true, true, false]
    ge: [true, true, true, true, true, false, true, true, true]
    lt: [true, true, true, true, false, true, true, false, false]
    le: [true, true, true, true, true, true, true, false, true]
  status: SATISFIED
***/

array [1..36] of float: addn :: add_to_output = [x + y | x, y in [<>, -2.0, -1.0, 0.0, 1.0, 2.0]];
array [1..36] of opt float: subn :: add_to_output = [x - y | x, y in [<>, -2.0, -1.0, 0.0, 1.0, 2.0]];
array [1..36] of float: prod :: add_to_output = [x * y | x, y in [<>, -2.0, -1.0, 0.0, 1.0, 2.0]];
array [1..25] of opt float: divn :: add_to_output = [x / y | x, y in [<>, -2.0, -1.0, 1.0, 2.0]];

array [1..9] of bool: gt :: add_to_output = [ x > y | x, y in [<>, 0.0, 1.0]];
array [1..9] of bool: ge :: add_to_output = [ x >= y | x, y in [<>, 0.0, 1.0]];
array [1..9] of bool: lt :: add_to_output = [ x < y | x, y in [<>, 0.0, 1.0]];
array [1..9] of bool: le :: add_to_output = [ x <= y | x, y in [<>, 0.0, 1.0]];

solve satisfy;
