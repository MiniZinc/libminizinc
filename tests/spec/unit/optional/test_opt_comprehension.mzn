/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    z:
    - false
    - false
    - false
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

array[1..3] of var bool: z;
any: x = [ if i>=2 then <> else 0 endif | i in index_set(z) ];
predicate occ(array[int] of int: x) = abort("Error: array contains <>");
predicate occ(array[int] of opt int: x) = true;
constraint occ(x);
