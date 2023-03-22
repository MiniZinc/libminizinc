/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    x: !!python/object:minizinc.types.ConstrEnum
      argument: 6
      constructor: S
  status: SATISFIED
extra_files: []
markers: []
name: ''
options:
  all_solutions: false
solvers:
- gecode
type: solve

***/

enum X = S({2,4,6});
X: x ::output = S(6);
