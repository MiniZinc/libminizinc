/***
!Test
check_against: []
expected:
- !Result
  solution:
  - !Solution
    x:
    - true
    - false
  - !Solution
    x:
    - true
    - true
  status: ALL_SOLUTIONS
extra_files: []
markers: []
name: ''
options:
  all_solutions: true
solvers:
- gecode
type: solve
***/

predicate f(array[$$E] of var bool: x) = let {
    any: y = index_set(x);
} in x[min(y)];

enum E = {a,b};
array[E] of var bool: x;
constraint f(x);
