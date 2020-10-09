/***
--- !Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    y:
    - - 1
      - 1
    - - 1
      - 1
  - !Solution
    y:
    - - 1
      - 2
    - - 2
      - 1
  - !Solution
    y:
    - - 1
      - 1
    - - 1
      - 2
  - !Solution
    y:
    - - 1
      - 2
    - - 1
      - 2
  - !Solution
    y:
    - - 1
      - 1
    - - 2
      - 2
  - !Solution
    y:
    - - 1
      - 2
    - - 2
      - 2
  - !Solution
    y:
    - - 2
      - 2
    - - 2
      - 2
options:
  all_solutions: true
--- !Test
solvers: [cbc]
expected:
- !Result
  solution: !Solution
    y:
    - [1, 1]
    - [1, 1]
- !Result
  solution: !Solution
    y:
    - [1, 2]
    - [2, 1]
- !Result
  solution: !Solution
    y:
    - [1, 1]
    - [1, 2]
- !Result
  solution: !Solution
    y:
    - [1, 2]
    - [1, 2]
- !Result
  solution: !Solution
    y:
    - [1, 1]
    - [2, 2]
- !Result
  solution: !Solution
    y:
    - [1, 2]
    - [2, 2]
- !Result
  solution: !Solution
    y:
    - [2, 2]
    - [2, 2]
***/

include "lex2.mzn";

array[1..2, 1..2] of var 1..2: y ::add_to_output;

constraint lex2([|2, 2, 3|
                  2, 3, 1|]);

constraint lex2(y);

solve satisfy;

output ["y = array2d(1..2, 1..2, ", show(y), ");\n"];
