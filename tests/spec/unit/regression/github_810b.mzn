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
var bool: c :: no_output;
var 0..10: l :: no_output;

constraint ((l in [0,1,2]) >= l) -> p;
constraint ((a -> b) >= l ) -> p;
constraint ((not a \/ not b \/ not c) >= 1) -> p;

output let {
  constraint assert(fix(((l in [0,1,2]) >= l) -> p), "Incorrect");
  constraint assert(fix(((a -> b) >= l ) -> p), "Incorrect");
  constraint assert(fix(((not a \/ not b \/ not c) >= 1) -> p), "Incorrect");
} in "Ok";
