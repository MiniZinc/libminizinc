/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    test_1: 1
    test_2: 3
***/

array[1..3] of var 1..3: a;
var -100..100: test_1 :: add_to_output = lb(a[1]);
var -100..100: test_2 :: add_to_output = ub(a[1]);

constraint lb_array(a) = 1;
constraint ub_array(a) = 3;

solve satisfy;
