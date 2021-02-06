/***
!Test
check_against: []
expected:
- !FlatZinc test_undefined_enum.fzn
extra_files: []
markers: []
options:
  --model-check-only: true
  all_solutions: false
solvers:
- gecode
type: compile

***/

enum A;
enum B = AB(A);

