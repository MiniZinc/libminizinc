/***
!Test
solvers: [gecode, chuffed, cbc]
expected: !Result
  solution: !Solution
    addn: [0, -2, -1, 0, 1, 2, -2, -4, -3, -2, -1, 0, -1, -3, -2, -1, 0, 1, 0, -2, -1, 0, 1, 2, 1, -1, 0, 1, 2, 3, 2, 0, 1, 2, 3, 4]
    subn: [null, null, null, null, null, null, -2, 0, -1, -2, -3, -4, -1, 1, 0, -1, -2, -3, 0, 2, 1, 0, -1, -2, 1, 3, 2, 1, 0, -1, 2, 4, 3, 2, 1, 0]
    prod: [1, -2, -1, 0, 1, 2, -2, 4, 2, 0, -2, -4, -1, 2, 1, 0, -1, -2, 0, 0, 0, 0, 0, 0, 1, -2, -1, 0, 1, 2, 2, -4, -2, 0, 2, 4]
    divn: [null, null, null, null, null, -2, 1, 2, -2, -1, -1, 0, 1, -1, 0, 1, 0, -1, 1, 0, 2, -1, -2, 2, 1]
    gt: [true, true, true, true, false, false, true, true, false]
    ge: [true, true, true, true, true, false, true, true, true]
    lt: [true, true, true, true, false, true, true, false, false]
    le: [true, true, true, true, true, true, true, false, true]
  status: SATISFIED
***/

array [1..36] of int: addn :: add_to_output = [x + y | x, y in [<>, -2, -1, 0, 1, 2]];
array [1..36] of opt int: subn :: add_to_output = [x - y | x, y in [<>, -2, -1, 0, 1, 2]];
array [1..36] of int: prod :: add_to_output = [x * y | x, y in [<>, -2, -1, 0, 1, 2]];
array [1..25] of opt int: divn :: add_to_output = [x div y | x, y in [<>, -2, -1, 1, 2]];

array [1..9] of bool: gt :: add_to_output = [ x > y | x, y in [<>, 0, 1]];
array [1..9] of bool: ge :: add_to_output = [ x >= y | x, y in [<>, 0, 1]];
array [1..9] of bool: lt :: add_to_output = [ x < y | x, y in [<>, 0, 1]];
array [1..9] of bool: le :: add_to_output = [ x <= y | x, y in [<>, 0, 1]];

solve satisfy;
