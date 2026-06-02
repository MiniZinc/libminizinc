/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    a_1: 1
    a_2: 1
    a_3: 1
    a_4: 1
    a_5: 1
    a_7: 1
    a_8: 1
  - !Solution
    a_1: 2
    a_2: 2
    a_3: 2
    a_4: 2
    a_5: 2
    a_7: 2
    a_8: 2
options:
  all_solutions: true
***/

% Chains of equalities between output variables used to collapse into a cyclic
% alias (e.g. `a_1 = a_3` and `a_3 = a_1`) during FlatZinc finalisation,
% depending on the order of the equality constraints. This produced
% "undefined identifier"/"Invalid var int initializer" errors.

int : nb_regs = 2;

var 1..nb_regs: a_1;
var 1..nb_regs: a_2;
var 1..nb_regs: a_3;
var 1..nb_regs: a_4;
var 1..nb_regs: a_5;
var 1..nb_regs: a_7;
var 1..nb_regs: a_8;

constraint a_1=a_2;

constraint a_1=a_7;
constraint a_1=a_8;

constraint a_1=a_3;
constraint a_1=a_4;
constraint a_1=a_5;
