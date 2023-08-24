/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution {}
  status: SATISFIED
***/

enum b = { e1 }; % Error: type-inst must be par set but is 'bool'
enum x = { e2 }; % Error: type-inst cannot be array
