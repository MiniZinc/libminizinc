/***
!Test
check_against: []
expected:
- !Result
  solution: !Solution
    as:
    - !!python/object:minizinc.types.ConstrEnum
      argument: z2
      constructor: z
    - !!python/object:minizinc.types.ConstrEnum
      argument: z1
      constructor: z
    - !!python/object:minizinc.types.ConstrEnum
      argument: x2
      constructor: x
    - !!python/object:minizinc.types.ConstrEnum
      argument: x1
      constructor: x
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

enum X = { x1, x2 }; 
enum Y = {};
enum Z = {z1, z2 };

enum A = x(X) ++ y(Y) ++ z(Z);

array[A] of var A: as;
include "all_different.mzn";
constraint all_different(as);
