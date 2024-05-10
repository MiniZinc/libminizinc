/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution:
  - !Solution
    _output_item: Ok
***/

var bool: p :: no_output;
var bool: a :: no_output;
var bool: b :: no_output;

constraint ((a \/ not b) >= 1) -> p;

output let {
  constraint assert(fix(((a \/ not b) >= 1) -> p), "Incorrect");
} in "Ok";
