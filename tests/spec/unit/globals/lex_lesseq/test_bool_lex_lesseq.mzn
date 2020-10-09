/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    x:
    - false
    - false
    y:
    - false
    - false
  - !Solution
    x:
    - false
    - false
    y:
    - true
    - false
  - !Solution
    x:
    - true
    - false
    y:
    - true
    - false
  - !Solution
    x:
    - false
    - true
    y:
    - true
    - false
  - !Solution
    x:
    - false
    - false
    y:
    - false
    - true
  - !Solution
    x:
    - false
    - true
    y:
    - false
    - true
  - !Solution
    x:
    - false
    - false
    y:
    - true
    - true
  - !Solution
    x:
    - true
    - false
    y:
    - true
    - true
  - !Solution
    x:
    - false
    - true
    y:
    - true
    - true
  - !Solution
    x:
    - true
    - true
    y:
    - true
    - true
options:
  all_solutions: true
***/

include "lex_lesseq.mzn";

array[1..2] of var bool: x ::add_to_output;
array[1..2] of var bool: y ::add_to_output;

constraint lex_lesseq(x, y);

solve satisfy;

output [
	"x = ", show(x), "\n",
	"y = ", show(y), "\n"
];
