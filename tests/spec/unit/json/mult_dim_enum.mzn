/***
!Test
extra_files:
- mult_dim_enum.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    use_e: [[A,B],[B,C]]
***/

enum E;
array[1..2,int] of E: use_e ::output;