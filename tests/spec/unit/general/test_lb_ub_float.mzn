/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    test_1: 3.0
    test_2: 3.0
    test_3: 1.0
    test_4: 10.0
***/

var 1.0..10.0: x;

var float: test_1 :: add_to_output = lb(3.0);
var float: test_2 :: add_to_output = ub(3.0);

var float: test_3 :: add_to_output = lb(x);
var float: test_4 :: add_to_output = ub(x);

solve satisfy;
