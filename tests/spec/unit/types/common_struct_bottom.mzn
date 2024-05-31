/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x:
    - - - - 1
    - - []
    "y":
    - a:
      - a: 1
    - a: []
***/

array [int] of tuple(array [int] of tuple(int)): x :: output = [([(1,)],), ([],)];
array [int] of record(array [int] of record(int: a): a): y :: output = [(a: [(a: 1)]), (a: [])];
