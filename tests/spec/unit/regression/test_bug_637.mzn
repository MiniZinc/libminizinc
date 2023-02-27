/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution {}
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

predicate foo(1..3: start) = 
  start = 3 \/
  let {
    int: bar = 1;
    int: start2 = start + 1;
  } in
    (foo(start2) /\ start2 = start2);

constraint foo(1);
