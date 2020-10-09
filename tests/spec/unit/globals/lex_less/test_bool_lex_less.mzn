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
options:
  all_solutions: true
***/

include "lex_less.mzn";

array[1..2] of var bool: x ::add_to_output;
array[1..2] of var bool: y ::add_to_output;

constraint lex_less(x, y);

solve satisfy;

output [
	"x = array1d(1..2, ", show(x), ");\n",
	"y = array1d(1..2, ", show(y), ");\n"
];
