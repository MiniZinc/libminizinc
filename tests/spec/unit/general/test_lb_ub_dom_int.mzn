/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    test_1: 3
    test_2: 3
    test_3: 1
    test_4: 10
***/

var 1..10: x;

var -100..100: test_1 :: add_to_output = lb(3);
var -100..100: test_2 :: add_to_output = ub(3);

var -100..100: test_3 :: add_to_output = lb(x);
var -100..100: test_4 :: add_to_output = ub(x);

constraint dom(x) = 1..10;

solve satisfy;

output [
   "test_1 = ", show(test_1), ";\n",
   "test_2 = ", show(test_2), ";\n",
   "test_3 = ", show(test_3), ";\n",
   "test_4 = ", show(test_4), ";\n"
];
