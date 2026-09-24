/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    t: [false, !!set {}]
  - !Solution
    t: [false, !!set {1}]
  - !Solution
    t: [false, !!set {2}]
  - !Solution
    t: [false, !!set {1, 2}]
  - !Solution
    t: [true, !!set {1}]
  - !Solution
    t: [true, !!set {1, 2}]
  status: ALL_SOLUTIONS
***/

% A set variable that is only used in the output must not end up in the FlatZinc
% for a solver without set support (chuffed uses nosets.mzn)
function tuple(var bool, var set of int): mk() =
  let {
    var bool: b;
    var set of 1..2: s;
    constraint b -> 1 in s;
  } in (b, s);
any: t :: output = mk();
