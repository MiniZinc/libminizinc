/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: "promise_total" 
    y: "expression_name(\"test\")" 
***/
ann: x ::add_to_output = promise_total;
ann: y ::add_to_output = expression_name("test");
