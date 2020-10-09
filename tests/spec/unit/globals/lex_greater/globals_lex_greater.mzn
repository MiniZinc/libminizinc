/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    lex_avi1: [1, 3, 5, 7, 9]
    lex_avi2: [1, 3, 5, 1, 9]
  - !Solution
    lex_avi1: [1, 3, 5, 7, 9]
    lex_avi2: [1, 3, 5, 2, 9]
  - !Solution
    lex_avi1: [1, 3, 5, 7, 9]
    lex_avi2: [1, 3, 5, 3, 9]
  - !Solution
    lex_avi1: [1, 3, 5, 7, 9]
    lex_avi2: [1, 3, 5, 4, 9]
  - !Solution
    lex_avi1: [1, 3, 5, 7, 9]
    lex_avi2: [1, 3, 5, 5, 9]
  - !Solution
    lex_avi1: [1, 3, 5, 7, 9]
    lex_avi2: [1, 3, 5, 6, 9]
options:
  all_solutions: true
***/

include "lex_greater.mzn";

%-----------------------------------------------------------------------------%
% lex_less
%-----------------------------------------------------------------------------%
array[1..5] of var -100..100: lex_avi1 ::add_to_output = [1,3,5,7,9];
array[4..8] of var 1..10: lex_avi2 ::add_to_output = array1d(4..8, [1,3,5,_,9]);

constraint lex_greater(lex_avi1, lex_avi2);
constraint lex_greater([5, 2, 8, 9], [5, 2, 6, 8]);
%constraint lex_greater([5, 2, 3, 9], [5, 2, 3, 9]);

solve satisfy;

output [
   "lex_avi1 = array1d(1..5, ", show(lex_avi1), ");\n",
   "lex_avi2 = array1d(4..8, ", show(lex_avi2), ");\n"
];
