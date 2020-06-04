/***
!Test
solvers: [gecode]
extra_files: [parse_assignments.mzc.mzn]
expected:
- !Result
  solution: !Solution
    _checker: >
      Ok
      
***/

% Used to cause an internal error due to copying lhs onto rhs when parsing solutions (making x = true cv in the checker)

var bool: x;
