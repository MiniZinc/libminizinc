/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    bins: [4, 4, 3, 3, 2, 1]
  - !Solution
    bins: [5, 4, 3, 3, 2, 1]
  - !Solution
    bins: [4, 5, 3, 3, 2, 1]
  - !Solution
    bins: [5, 5, 3, 3, 2, 1]
  - !Solution
    bins: [4, 3, 4, 3, 2, 1]
  - !Solution
    bins: [5, 3, 4, 3, 2, 1]
  - !Solution
    bins: [3, 4, 4, 3, 2, 1]
  - !Solution
    bins: [4, 4, 4, 3, 2, 1]
  - !Solution
    bins: [5, 4, 4, 3, 2, 1]
  - !Solution
    bins: [3, 5, 4, 3, 2, 1]
  - !Solution
    bins: [4, 5, 4, 3, 2, 1]
  - !Solution
    bins: [5, 5, 4, 3, 2, 1]
  - !Solution
    bins: [4, 3, 5, 3, 2, 1]
  - !Solution
    bins: [5, 3, 5, 3, 2, 1]
  - !Solution
    bins: [3, 4, 5, 3, 2, 1]
  - !Solution
    bins: [4, 4, 5, 3, 2, 1]
  - !Solution
    bins: [5, 4, 5, 3, 2, 1]
  - !Solution
    bins: [3, 5, 5, 3, 2, 1]
  - !Solution
    bins: [4, 5, 5, 3, 2, 1]
  - !Solution
    bins: [5, 5, 5, 3, 2, 1]
  - !Solution
    bins: [3, 3, 3, 4, 2, 1]
  - !Solution
    bins: [4, 3, 3, 4, 2, 1]
  - !Solution
    bins: [5, 3, 3, 4, 2, 1]
  - !Solution
    bins: [3, 4, 3, 4, 2, 1]
  - !Solution
    bins: [5, 4, 3, 4, 2, 1]
  - !Solution
    bins: [3, 5, 3, 4, 2, 1]
  - !Solution
    bins: [4, 5, 3, 4, 2, 1]
  - !Solution
    bins: [5, 5, 3, 4, 2, 1]
  - !Solution
    bins: [3, 3, 4, 4, 2, 1]
  - !Solution
    bins: [5, 3, 4, 4, 2, 1]
  - !Solution
    bins: [3, 5, 4, 4, 2, 1]
  - !Solution
    bins: [5, 5, 4, 4, 2, 1]
  - !Solution
    bins: [3, 3, 5, 4, 2, 1]
  - !Solution
    bins: [4, 3, 5, 4, 2, 1]
  - !Solution
    bins: [5, 3, 5, 4, 2, 1]
  - !Solution
    bins: [3, 4, 5, 4, 2, 1]
  - !Solution
    bins: [5, 4, 5, 4, 2, 1]
  - !Solution
    bins: [3, 5, 5, 4, 2, 1]
  - !Solution
    bins: [4, 5, 5, 4, 2, 1]
  - !Solution
    bins: [5, 5, 5, 4, 2, 1]
  - !Solution
    bins: [3, 3, 3, 5, 2, 1]
  - !Solution
    bins: [4, 3, 3, 5, 2, 1]
  - !Solution
    bins: [5, 3, 3, 5, 2, 1]
  - !Solution
    bins: [3, 4, 3, 5, 2, 1]
  - !Solution
    bins: [4, 4, 3, 5, 2, 1]
  - !Solution
    bins: [5, 4, 3, 5, 2, 1]
  - !Solution
    bins: [3, 5, 3, 5, 2, 1]
  - !Solution
    bins: [4, 5, 3, 5, 2, 1]
  - !Solution
    bins: [3, 3, 4, 5, 2, 1]
  - !Solution
    bins: [4, 3, 4, 5, 2, 1]
  - !Solution
    bins: [5, 3, 4, 5, 2, 1]
  - !Solution
    bins: [3, 4, 4, 5, 2, 1]
  - !Solution
    bins: [4, 4, 4, 5, 2, 1]
  - !Solution
    bins: [5, 4, 4, 5, 2, 1]
  - !Solution
    bins: [3, 5, 4, 5, 2, 1]
  - !Solution
    bins: [4, 5, 4, 5, 2, 1]
  - !Solution
    bins: [3, 3, 5, 5, 2, 1]
  - !Solution
    bins: [4, 3, 5, 5, 2, 1]
  - !Solution
    bins: [3, 4, 5, 5, 2, 1]
  - !Solution
    bins: [4, 4, 5, 5, 2, 1]
  - !Solution
    bins: [4, 4, 2, 2, 3, 1]
  - !Solution
    bins: [5, 4, 2, 2, 3, 1]
  - !Solution
    bins: [4, 5, 2, 2, 3, 1]
  - !Solution
    bins: [5, 5, 2, 2, 3, 1]
  - !Solution
    bins: [4, 2, 4, 2, 3, 1]
  - !Solution
    bins: [5, 2, 4, 2, 3, 1]
  - !Solution
    bins: [2, 4, 4, 2, 3, 1]
  - !Solution
    bins: [4, 4, 4, 2, 3, 1]
  - !Solution
    bins: [5, 4, 4, 2, 3, 1]
  - !Solution
    bins: [2, 5, 4, 2, 3, 1]
  - !Solution
    bins: [4, 5, 4, 2, 3, 1]
  - !Solution
    bins: [5, 5, 4, 2, 3, 1]
  - !Solution
    bins: [4, 2, 5, 2, 3, 1]
  - !Solution
    bins: [5, 2, 5, 2, 3, 1]
  - !Solution
    bins: [2, 4, 5, 2, 3, 1]
  - !Solution
    bins: [4, 4, 5, 2, 3, 1]
  - !Solution
    bins: [5, 4, 5, 2, 3, 1]
  - !Solution
    bins: [2, 5, 5, 2, 3, 1]
  - !Solution
    bins: [4, 5, 5, 2, 3, 1]
  - !Solution
    bins: [5, 5, 5, 2, 3, 1]
  - !Solution
    bins: [2, 2, 2, 4, 3, 1]
  - !Solution
    bins: [4, 2, 2, 4, 3, 1]
  - !Solution
    bins: [5, 2, 2, 4, 3, 1]
  - !Solution
    bins: [2, 4, 2, 4, 3, 1]
  - !Solution
    bins: [5, 4, 2, 4, 3, 1]
  - !Solution
    bins: [2, 5, 2, 4, 3, 1]
  - !Solution
    bins: [4, 5, 2, 4, 3, 1]
  - !Solution
    bins: [5, 5, 2, 4, 3, 1]
  - !Solution
    bins: [2, 2, 4, 4, 3, 1]
  - !Solution
    bins: [5, 2, 4, 4, 3, 1]
  - !Solution
    bins: [2, 5, 4, 4, 3, 1]
  - !Solution
    bins: [5, 5, 4, 4, 3, 1]
  - !Solution
    bins: [2, 2, 5, 4, 3, 1]
  - !Solution
    bins: [4, 2, 5, 4, 3, 1]
  - !Solution
    bins: [5, 2, 5, 4, 3, 1]
  - !Solution
    bins: [2, 4, 5, 4, 3, 1]
  - !Solution
    bins: [5, 4, 5, 4, 3, 1]
  - !Solution
    bins: [2, 5, 5, 4, 3, 1]
  - !Solution
    bins: [4, 5, 5, 4, 3, 1]
  - !Solution
    bins: [5, 5, 5, 4, 3, 1]
  - !Solution
    bins: [2, 2, 2, 5, 3, 1]
  - !Solution
    bins: [4, 2, 2, 5, 3, 1]
  - !Solution
    bins: [5, 2, 2, 5, 3, 1]
  - !Solution
    bins: [2, 4, 2, 5, 3, 1]
  - !Solution
    bins: [4, 4, 2, 5, 3, 1]
  - !Solution
    bins: [5, 4, 2, 5, 3, 1]
  - !Solution
    bins: [2, 5, 2, 5, 3, 1]
  - !Solution
    bins: [4, 5, 2, 5, 3, 1]
  - !Solution
    bins: [2, 2, 4, 5, 3, 1]
  - !Solution
    bins: [4, 2, 4, 5, 3, 1]
  - !Solution
    bins: [5, 2, 4, 5, 3, 1]
  - !Solution
    bins: [2, 4, 4, 5, 3, 1]
  - !Solution
    bins: [4, 4, 4, 5, 3, 1]
  - !Solution
    bins: [5, 4, 4, 5, 3, 1]
  - !Solution
    bins: [2, 5, 4, 5, 3, 1]
  - !Solution
    bins: [4, 5, 4, 5, 3, 1]
  - !Solution
    bins: [2, 2, 5, 5, 3, 1]
  - !Solution
    bins: [4, 2, 5, 5, 3, 1]
  - !Solution
    bins: [2, 4, 5, 5, 3, 1]
  - !Solution
    bins: [4, 4, 5, 5, 3, 1]
  - !Solution
    bins: [3, 3, 2, 2, 4, 1]
  - !Solution
    bins: [5, 3, 2, 2, 4, 1]
  - !Solution
    bins: [3, 5, 2, 2, 4, 1]
  - !Solution
    bins: [5, 5, 2, 2, 4, 1]
  - !Solution
    bins: [3, 2, 3, 2, 4, 1]
  - !Solution
    bins: [5, 2, 3, 2, 4, 1]
  - !Solution
    bins: [2, 3, 3, 2, 4, 1]
  - !Solution
    bins: [3, 3, 3, 2, 4, 1]
  - !Solution
    bins: [5, 3, 3, 2, 4, 1]
  - !Solution
    bins: [2, 5, 3, 2, 4, 1]
  - !Solution
    bins: [3, 5, 3, 2, 4, 1]
  - !Solution
    bins: [5, 5, 3, 2, 4, 1]
  - !Solution
    bins: [3, 2, 5, 2, 4, 1]
  - !Solution
    bins: [5, 2, 5, 2, 4, 1]
  - !Solution
    bins: [2, 3, 5, 2, 4, 1]
  - !Solution
    bins: [3, 3, 5, 2, 4, 1]
  - !Solution
    bins: [5, 3, 5, 2, 4, 1]
  - !Solution
    bins: [2, 5, 5, 2, 4, 1]
  - !Solution
    bins: [3, 5, 5, 2, 4, 1]
  - !Solution
    bins: [5, 5, 5, 2, 4, 1]
  - !Solution
    bins: [2, 2, 2, 3, 4, 1]
  - !Solution
    bins: [3, 2, 2, 3, 4, 1]
  - !Solution
    bins: [5, 2, 2, 3, 4, 1]
  - !Solution
    bins: [2, 3, 2, 3, 4, 1]
  - !Solution
    bins: [5, 3, 2, 3, 4, 1]
  - !Solution
    bins: [2, 5, 2, 3, 4, 1]
  - !Solution
    bins: [3, 5, 2, 3, 4, 1]
  - !Solution
    bins: [5, 5, 2, 3, 4, 1]
  - !Solution
    bins: [2, 2, 3, 3, 4, 1]
  - !Solution
    bins: [5, 2, 3, 3, 4, 1]
  - !Solution
    bins: [2, 5, 3, 3, 4, 1]
  - !Solution
    bins: [5, 5, 3, 3, 4, 1]
  - !Solution
    bins: [2, 2, 5, 3, 4, 1]
  - !Solution
    bins: [3, 2, 5, 3, 4, 1]
  - !Solution
    bins: [5, 2, 5, 3, 4, 1]
  - !Solution
    bins: [2, 3, 5, 3, 4, 1]
  - !Solution
    bins: [5, 3, 5, 3, 4, 1]
  - !Solution
    bins: [2, 5, 5, 3, 4, 1]
  - !Solution
    bins: [3, 5, 5, 3, 4, 1]
  - !Solution
    bins: [5, 5, 5, 3, 4, 1]
  - !Solution
    bins: [2, 2, 2, 5, 4, 1]
  - !Solution
    bins: [3, 2, 2, 5, 4, 1]
  - !Solution
    bins: [5, 2, 2, 5, 4, 1]
  - !Solution
    bins: [2, 3, 2, 5, 4, 1]
  - !Solution
    bins: [3, 3, 2, 5, 4, 1]
  - !Solution
    bins: [5, 3, 2, 5, 4, 1]
  - !Solution
    bins: [2, 5, 2, 5, 4, 1]
  - !Solution
    bins: [3, 5, 2, 5, 4, 1]
  - !Solution
    bins: [2, 2, 3, 5, 4, 1]
  - !Solution
    bins: [3, 2, 3, 5, 4, 1]
  - !Solution
    bins: [5, 2, 3, 5, 4, 1]
  - !Solution
    bins: [2, 3, 3, 5, 4, 1]
  - !Solution
    bins: [3, 3, 3, 5, 4, 1]
  - !Solution
    bins: [5, 3, 3, 5, 4, 1]
  - !Solution
    bins: [2, 5, 3, 5, 4, 1]
  - !Solution
    bins: [3, 5, 3, 5, 4, 1]
  - !Solution
    bins: [2, 2, 5, 5, 4, 1]
  - !Solution
    bins: [3, 2, 5, 5, 4, 1]
  - !Solution
    bins: [2, 3, 5, 5, 4, 1]
  - !Solution
    bins: [3, 3, 5, 5, 4, 1]
  - !Solution
    bins: [3, 3, 2, 2, 5, 1]
  - !Solution
    bins: [4, 3, 2, 2, 5, 1]
  - !Solution
    bins: [3, 4, 2, 2, 5, 1]
  - !Solution
    bins: [4, 4, 2, 2, 5, 1]
  - !Solution
    bins: [3, 2, 3, 2, 5, 1]
  - !Solution
    bins: [4, 2, 3, 2, 5, 1]
  - !Solution
    bins: [2, 3, 3, 2, 5, 1]
  - !Solution
    bins: [3, 3, 3, 2, 5, 1]
  - !Solution
    bins: [4, 3, 3, 2, 5, 1]
  - !Solution
    bins: [2, 4, 3, 2, 5, 1]
  - !Solution
    bins: [3, 4, 3, 2, 5, 1]
  - !Solution
    bins: [4, 4, 3, 2, 5, 1]
  - !Solution
    bins: [3, 2, 4, 2, 5, 1]
  - !Solution
    bins: [4, 2, 4, 2, 5, 1]
  - !Solution
    bins: [2, 3, 4, 2, 5, 1]
  - !Solution
    bins: [3, 3, 4, 2, 5, 1]
  - !Solution
    bins: [4, 3, 4, 2, 5, 1]
  - !Solution
    bins: [2, 4, 4, 2, 5, 1]
  - !Solution
    bins: [3, 4, 4, 2, 5, 1]
  - !Solution
    bins: [4, 4, 4, 2, 5, 1]
  - !Solution
    bins: [2, 2, 2, 3, 5, 1]
  - !Solution
    bins: [3, 2, 2, 3, 5, 1]
  - !Solution
    bins: [4, 2, 2, 3, 5, 1]
  - !Solution
    bins: [2, 3, 2, 3, 5, 1]
  - !Solution
    bins: [4, 3, 2, 3, 5, 1]
  - !Solution
    bins: [2, 4, 2, 3, 5, 1]
  - !Solution
    bins: [3, 4, 2, 3, 5, 1]
  - !Solution
    bins: [4, 4, 2, 3, 5, 1]
  - !Solution
    bins: [2, 2, 3, 3, 5, 1]
  - !Solution
    bins: [4, 2, 3, 3, 5, 1]
  - !Solution
    bins: [2, 4, 3, 3, 5, 1]
  - !Solution
    bins: [4, 4, 3, 3, 5, 1]
  - !Solution
    bins: [2, 2, 4, 3, 5, 1]
  - !Solution
    bins: [3, 2, 4, 3, 5, 1]
  - !Solution
    bins: [4, 2, 4, 3, 5, 1]
  - !Solution
    bins: [2, 3, 4, 3, 5, 1]
  - !Solution
    bins: [4, 3, 4, 3, 5, 1]
  - !Solution
    bins: [2, 4, 4, 3, 5, 1]
  - !Solution
    bins: [3, 4, 4, 3, 5, 1]
  - !Solution
    bins: [4, 4, 4, 3, 5, 1]
  - !Solution
    bins: [2, 2, 2, 4, 5, 1]
  - !Solution
    bins: [3, 2, 2, 4, 5, 1]
  - !Solution
    bins: [4, 2, 2, 4, 5, 1]
  - !Solution
    bins: [2, 3, 2, 4, 5, 1]
  - !Solution
    bins: [3, 3, 2, 4, 5, 1]
  - !Solution
    bins: [4, 3, 2, 4, 5, 1]
  - !Solution
    bins: [2, 4, 2, 4, 5, 1]
  - !Solution
    bins: [3, 4, 2, 4, 5, 1]
  - !Solution
    bins: [2, 2, 3, 4, 5, 1]
  - !Solution
    bins: [3, 2, 3, 4, 5, 1]
  - !Solution
    bins: [4, 2, 3, 4, 5, 1]
  - !Solution
    bins: [2, 3, 3, 4, 5, 1]
  - !Solution
    bins: [3, 3, 3, 4, 5, 1]
  - !Solution
    bins: [4, 3, 3, 4, 5, 1]
  - !Solution
    bins: [2, 4, 3, 4, 5, 1]
  - !Solution
    bins: [3, 4, 3, 4, 5, 1]
  - !Solution
    bins: [2, 2, 4, 4, 5, 1]
  - !Solution
    bins: [3, 2, 4, 4, 5, 1]
  - !Solution
    bins: [2, 3, 4, 4, 5, 1]
  - !Solution
    bins: [3, 3, 4, 4, 5, 1]
  - !Solution
    bins: [4, 4, 3, 3, 1, 2]
  - !Solution
    bins: [5, 4, 3, 3, 1, 2]
  - !Solution
    bins: [4, 5, 3, 3, 1, 2]
  - !Solution
    bins: [5, 5, 3, 3, 1, 2]
  - !Solution
    bins: [4, 3, 4, 3, 1, 2]
  - !Solution
    bins: [5, 3, 4, 3, 1, 2]
  - !Solution
    bins: [3, 4, 4, 3, 1, 2]
  - !Solution
    bins: [4, 4, 4, 3, 1, 2]
  - !Solution
    bins: [5, 4, 4, 3, 1, 2]
  - !Solution
    bins: [3, 5, 4, 3, 1, 2]
  - !Solution
    bins: [4, 5, 4, 3, 1, 2]
  - !Solution
    bins: [5, 5, 4, 3, 1, 2]
  - !Solution
    bins: [4, 3, 5, 3, 1, 2]
  - !Solution
    bins: [5, 3, 5, 3, 1, 2]
  - !Solution
    bins: [3, 4, 5, 3, 1, 2]
  - !Solution
    bins: [4, 4, 5, 3, 1, 2]
  - !Solution
    bins: [5, 4, 5, 3, 1, 2]
  - !Solution
    bins: [3, 5, 5, 3, 1, 2]
  - !Solution
    bins: [4, 5, 5, 3, 1, 2]
  - !Solution
    bins: [5, 5, 5, 3, 1, 2]
  - !Solution
    bins: [3, 3, 3, 4, 1, 2]
  - !Solution
    bins: [4, 3, 3, 4, 1, 2]
  - !Solution
    bins: [5, 3, 3, 4, 1, 2]
  - !Solution
    bins: [3, 4, 3, 4, 1, 2]
  - !Solution
    bins: [5, 4, 3, 4, 1, 2]
  - !Solution
    bins: [3, 5, 3, 4, 1, 2]
  - !Solution
    bins: [4, 5, 3, 4, 1, 2]
  - !Solution
    bins: [5, 5, 3, 4, 1, 2]
  - !Solution
    bins: [3, 3, 4, 4, 1, 2]
  - !Solution
    bins: [5, 3, 4, 4, 1, 2]
  - !Solution
    bins: [3, 5, 4, 4, 1, 2]
  - !Solution
    bins: [5, 5, 4, 4, 1, 2]
  - !Solution
    bins: [3, 3, 5, 4, 1, 2]
  - !Solution
    bins: [4, 3, 5, 4, 1, 2]
  - !Solution
    bins: [5, 3, 5, 4, 1, 2]
  - !Solution
    bins: [3, 4, 5, 4, 1, 2]
  - !Solution
    bins: [5, 4, 5, 4, 1, 2]
  - !Solution
    bins: [3, 5, 5, 4, 1, 2]
  - !Solution
    bins: [4, 5, 5, 4, 1, 2]
  - !Solution
    bins: [5, 5, 5, 4, 1, 2]
  - !Solution
    bins: [3, 3, 3, 5, 1, 2]
  - !Solution
    bins: [4, 3, 3, 5, 1, 2]
  - !Solution
    bins: [5, 3, 3, 5, 1, 2]
  - !Solution
    bins: [3, 4, 3, 5, 1, 2]
  - !Solution
    bins: [4, 4, 3, 5, 1, 2]
  - !Solution
    bins: [5, 4, 3, 5, 1, 2]
  - !Solution
    bins: [3, 5, 3, 5, 1, 2]
  - !Solution
    bins: [4, 5, 3, 5, 1, 2]
  - !Solution
    bins: [3, 3, 4, 5, 1, 2]
  - !Solution
    bins: [4, 3, 4, 5, 1, 2]
  - !Solution
    bins: [5, 3, 4, 5, 1, 2]
  - !Solution
    bins: [3, 4, 4, 5, 1, 2]
  - !Solution
    bins: [4, 4, 4, 5, 1, 2]
  - !Solution
    bins: [5, 4, 4, 5, 1, 2]
  - !Solution
    bins: [3, 5, 4, 5, 1, 2]
  - !Solution
    bins: [4, 5, 4, 5, 1, 2]
  - !Solution
    bins: [3, 3, 5, 5, 1, 2]
  - !Solution
    bins: [4, 3, 5, 5, 1, 2]
  - !Solution
    bins: [3, 4, 5, 5, 1, 2]
  - !Solution
    bins: [4, 4, 5, 5, 1, 2]
  - !Solution
    bins: [4, 4, 1, 1, 3, 2]
  - !Solution
    bins: [5, 4, 1, 1, 3, 2]
  - !Solution
    bins: [4, 5, 1, 1, 3, 2]
  - !Solution
    bins: [5, 5, 1, 1, 3, 2]
  - !Solution
    bins: [4, 1, 4, 1, 3, 2]
  - !Solution
    bins: [5, 1, 4, 1, 3, 2]
  - !Solution
    bins: [1, 4, 4, 1, 3, 2]
  - !Solution
    bins: [4, 4, 4, 1, 3, 2]
  - !Solution
    bins: [5, 4, 4, 1, 3, 2]
  - !Solution
    bins: [1, 5, 4, 1, 3, 2]
  - !Solution
    bins: [4, 5, 4, 1, 3, 2]
  - !Solution
    bins: [5, 5, 4, 1, 3, 2]
  - !Solution
    bins: [4, 1, 5, 1, 3, 2]
  - !Solution
    bins: [5, 1, 5, 1, 3, 2]
  - !Solution
    bins: [1, 4, 5, 1, 3, 2]
  - !Solution
    bins: [4, 4, 5, 1, 3, 2]
  - !Solution
    bins: [5, 4, 5, 1, 3, 2]
  - !Solution
    bins: [1, 5, 5, 1, 3, 2]
  - !Solution
    bins: [4, 5, 5, 1, 3, 2]
  - !Solution
    bins: [5, 5, 5, 1, 3, 2]
  - !Solution
    bins: [1, 1, 1, 4, 3, 2]
  - !Solution
    bins: [4, 1, 1, 4, 3, 2]
  - !Solution
    bins: [5, 1, 1, 4, 3, 2]
  - !Solution
    bins: [1, 4, 1, 4, 3, 2]
  - !Solution
    bins: [5, 4, 1, 4, 3, 2]
  - !Solution
    bins: [1, 5, 1, 4, 3, 2]
  - !Solution
    bins: [4, 5, 1, 4, 3, 2]
  - !Solution
    bins: [5, 5, 1, 4, 3, 2]
  - !Solution
    bins: [1, 1, 4, 4, 3, 2]
  - !Solution
    bins: [5, 1, 4, 4, 3, 2]
  - !Solution
    bins: [1, 5, 4, 4, 3, 2]
  - !Solution
    bins: [5, 5, 4, 4, 3, 2]
  - !Solution
    bins: [1, 1, 5, 4, 3, 2]
  - !Solution
    bins: [4, 1, 5, 4, 3, 2]
  - !Solution
    bins: [5, 1, 5, 4, 3, 2]
  - !Solution
    bins: [1, 4, 5, 4, 3, 2]
  - !Solution
    bins: [5, 4, 5, 4, 3, 2]
  - !Solution
    bins: [1, 5, 5, 4, 3, 2]
  - !Solution
    bins: [4, 5, 5, 4, 3, 2]
  - !Solution
    bins: [5, 5, 5, 4, 3, 2]
  - !Solution
    bins: [1, 1, 1, 5, 3, 2]
  - !Solution
    bins: [4, 1, 1, 5, 3, 2]
  - !Solution
    bins: [5, 1, 1, 5, 3, 2]
  - !Solution
    bins: [1, 4, 1, 5, 3, 2]
  - !Solution
    bins: [4, 4, 1, 5, 3, 2]
  - !Solution
    bins: [5, 4, 1, 5, 3, 2]
  - !Solution
    bins: [1, 5, 1, 5, 3, 2]
  - !Solution
    bins: [4, 5, 1, 5, 3, 2]
  - !Solution
    bins: [1, 1, 4, 5, 3, 2]
  - !Solution
    bins: [4, 1, 4, 5, 3, 2]
  - !Solution
    bins: [5, 1, 4, 5, 3, 2]
  - !Solution
    bins: [1, 4, 4, 5, 3, 2]
  - !Solution
    bins: [4, 4, 4, 5, 3, 2]
  - !Solution
    bins: [5, 4, 4, 5, 3, 2]
  - !Solution
    bins: [1, 5, 4, 5, 3, 2]
  - !Solution
    bins: [4, 5, 4, 5, 3, 2]
  - !Solution
    bins: [1, 1, 5, 5, 3, 2]
  - !Solution
    bins: [4, 1, 5, 5, 3, 2]
  - !Solution
    bins: [1, 4, 5, 5, 3, 2]
  - !Solution
    bins: [4, 4, 5, 5, 3, 2]
  - !Solution
    bins: [3, 3, 1, 1, 4, 2]
  - !Solution
    bins: [5, 3, 1, 1, 4, 2]
  - !Solution
    bins: [3, 5, 1, 1, 4, 2]
  - !Solution
    bins: [5, 5, 1, 1, 4, 2]
  - !Solution
    bins: [3, 1, 3, 1, 4, 2]
  - !Solution
    bins: [5, 1, 3, 1, 4, 2]
  - !Solution
    bins: [1, 3, 3, 1, 4, 2]
  - !Solution
    bins: [3, 3, 3, 1, 4, 2]
  - !Solution
    bins: [5, 3, 3, 1, 4, 2]
  - !Solution
    bins: [1, 5, 3, 1, 4, 2]
  - !Solution
    bins: [3, 5, 3, 1, 4, 2]
  - !Solution
    bins: [5, 5, 3, 1, 4, 2]
  - !Solution
    bins: [3, 1, 5, 1, 4, 2]
  - !Solution
    bins: [5, 1, 5, 1, 4, 2]
  - !Solution
    bins: [1, 3, 5, 1, 4, 2]
  - !Solution
    bins: [3, 3, 5, 1, 4, 2]
  - !Solution
    bins: [5, 3, 5, 1, 4, 2]
  - !Solution
    bins: [1, 5, 5, 1, 4, 2]
  - !Solution
    bins: [3, 5, 5, 1, 4, 2]
  - !Solution
    bins: [5, 5, 5, 1, 4, 2]
  - !Solution
    bins: [1, 1, 1, 3, 4, 2]
  - !Solution
    bins: [3, 1, 1, 3, 4, 2]
  - !Solution
    bins: [5, 1, 1, 3, 4, 2]
  - !Solution
    bins: [1, 3, 1, 3, 4, 2]
  - !Solution
    bins: [5, 3, 1, 3, 4, 2]
  - !Solution
    bins: [1, 5, 1, 3, 4, 2]
  - !Solution
    bins: [3, 5, 1, 3, 4, 2]
  - !Solution
    bins: [5, 5, 1, 3, 4, 2]
  - !Solution
    bins: [1, 1, 3, 3, 4, 2]
  - !Solution
    bins: [5, 1, 3, 3, 4, 2]
  - !Solution
    bins: [1, 5, 3, 3, 4, 2]
  - !Solution
    bins: [5, 5, 3, 3, 4, 2]
  - !Solution
    bins: [1, 1, 5, 3, 4, 2]
  - !Solution
    bins: [3, 1, 5, 3, 4, 2]
  - !Solution
    bins: [5, 1, 5, 3, 4, 2]
  - !Solution
    bins: [1, 3, 5, 3, 4, 2]
  - !Solution
    bins: [5, 3, 5, 3, 4, 2]
  - !Solution
    bins: [1, 5, 5, 3, 4, 2]
  - !Solution
    bins: [3, 5, 5, 3, 4, 2]
  - !Solution
    bins: [5, 5, 5, 3, 4, 2]
  - !Solution
    bins: [1, 1, 1, 5, 4, 2]
  - !Solution
    bins: [3, 1, 1, 5, 4, 2]
  - !Solution
    bins: [5, 1, 1, 5, 4, 2]
  - !Solution
    bins: [1, 3, 1, 5, 4, 2]
  - !Solution
    bins: [3, 3, 1, 5, 4, 2]
  - !Solution
    bins: [5, 3, 1, 5, 4, 2]
  - !Solution
    bins: [1, 5, 1, 5, 4, 2]
  - !Solution
    bins: [3, 5, 1, 5, 4, 2]
  - !Solution
    bins: [1, 1, 3, 5, 4, 2]
  - !Solution
    bins: [3, 1, 3, 5, 4, 2]
  - !Solution
    bins: [5, 1, 3, 5, 4, 2]
  - !Solution
    bins: [1, 3, 3, 5, 4, 2]
  - !Solution
    bins: [3, 3, 3, 5, 4, 2]
  - !Solution
    bins: [5, 3, 3, 5, 4, 2]
  - !Solution
    bins: [1, 5, 3, 5, 4, 2]
  - !Solution
    bins: [3, 5, 3, 5, 4, 2]
  - !Solution
    bins: [1, 1, 5, 5, 4, 2]
  - !Solution
    bins: [3, 1, 5, 5, 4, 2]
  - !Solution
    bins: [1, 3, 5, 5, 4, 2]
  - !Solution
    bins: [3, 3, 5, 5, 4, 2]
  - !Solution
    bins: [3, 3, 1, 1, 5, 2]
  - !Solution
    bins: [4, 3, 1, 1, 5, 2]
  - !Solution
    bins: [3, 4, 1, 1, 5, 2]
  - !Solution
    bins: [4, 4, 1, 1, 5, 2]
  - !Solution
    bins: [3, 1, 3, 1, 5, 2]
  - !Solution
    bins: [4, 1, 3, 1, 5, 2]
  - !Solution
    bins: [1, 3, 3, 1, 5, 2]
  - !Solution
    bins: [3, 3, 3, 1, 5, 2]
  - !Solution
    bins: [4, 3, 3, 1, 5, 2]
  - !Solution
    bins: [1, 4, 3, 1, 5, 2]
  - !Solution
    bins: [3, 4, 3, 1, 5, 2]
  - !Solution
    bins: [4, 4, 3, 1, 5, 2]
  - !Solution
    bins: [3, 1, 4, 1, 5, 2]
  - !Solution
    bins: [4, 1, 4, 1, 5, 2]
  - !Solution
    bins: [1, 3, 4, 1, 5, 2]
  - !Solution
    bins: [3, 3, 4, 1, 5, 2]
  - !Solution
    bins: [4, 3, 4, 1, 5, 2]
  - !Solution
    bins: [1, 4, 4, 1, 5, 2]
  - !Solution
    bins: [3, 4, 4, 1, 5, 2]
  - !Solution
    bins: [4, 4, 4, 1, 5, 2]
  - !Solution
    bins: [1, 1, 1, 3, 5, 2]
  - !Solution
    bins: [3, 1, 1, 3, 5, 2]
  - !Solution
    bins: [4, 1, 1, 3, 5, 2]
  - !Solution
    bins: [1, 3, 1, 3, 5, 2]
  - !Solution
    bins: [4, 3, 1, 3, 5, 2]
  - !Solution
    bins: [1, 4, 1, 3, 5, 2]
  - !Solution
    bins: [3, 4, 1, 3, 5, 2]
  - !Solution
    bins: [4, 4, 1, 3, 5, 2]
  - !Solution
    bins: [1, 1, 3, 3, 5, 2]
  - !Solution
    bins: [4, 1, 3, 3, 5, 2]
  - !Solution
    bins: [1, 4, 3, 3, 5, 2]
  - !Solution
    bins: [4, 4, 3, 3, 5, 2]
  - !Solution
    bins: [1, 1, 4, 3, 5, 2]
  - !Solution
    bins: [3, 1, 4, 3, 5, 2]
  - !Solution
    bins: [4, 1, 4, 3, 5, 2]
  - !Solution
    bins: [1, 3, 4, 3, 5, 2]
  - !Solution
    bins: [4, 3, 4, 3, 5, 2]
  - !Solution
    bins: [1, 4, 4, 3, 5, 2]
  - !Solution
    bins: [3, 4, 4, 3, 5, 2]
  - !Solution
    bins: [4, 4, 4, 3, 5, 2]
  - !Solution
    bins: [1, 1, 1, 4, 5, 2]
  - !Solution
    bins: [3, 1, 1, 4, 5, 2]
  - !Solution
    bins: [4, 1, 1, 4, 5, 2]
  - !Solution
    bins: [1, 3, 1, 4, 5, 2]
  - !Solution
    bins: [3, 3, 1, 4, 5, 2]
  - !Solution
    bins: [4, 3, 1, 4, 5, 2]
  - !Solution
    bins: [1, 4, 1, 4, 5, 2]
  - !Solution
    bins: [3, 4, 1, 4, 5, 2]
  - !Solution
    bins: [1, 1, 3, 4, 5, 2]
  - !Solution
    bins: [3, 1, 3, 4, 5, 2]
  - !Solution
    bins: [4, 1, 3, 4, 5, 2]
  - !Solution
    bins: [1, 3, 3, 4, 5, 2]
  - !Solution
    bins: [3, 3, 3, 4, 5, 2]
  - !Solution
    bins: [4, 3, 3, 4, 5, 2]
  - !Solution
    bins: [1, 4, 3, 4, 5, 2]
  - !Solution
    bins: [3, 4, 3, 4, 5, 2]
  - !Solution
    bins: [1, 1, 4, 4, 5, 2]
  - !Solution
    bins: [3, 1, 4, 4, 5, 2]
  - !Solution
    bins: [1, 3, 4, 4, 5, 2]
  - !Solution
    bins: [3, 3, 4, 4, 5, 2]
  - !Solution
    bins: [4, 4, 2, 2, 1, 3]
  - !Solution
    bins: [5, 4, 2, 2, 1, 3]
  - !Solution
    bins: [4, 5, 2, 2, 1, 3]
  - !Solution
    bins: [5, 5, 2, 2, 1, 3]
  - !Solution
    bins: [4, 2, 4, 2, 1, 3]
  - !Solution
    bins: [5, 2, 4, 2, 1, 3]
  - !Solution
    bins: [2, 4, 4, 2, 1, 3]
  - !Solution
    bins: [4, 4, 4, 2, 1, 3]
  - !Solution
    bins: [5, 4, 4, 2, 1, 3]
  - !Solution
    bins: [2, 5, 4, 2, 1, 3]
  - !Solution
    bins: [4, 5, 4, 2, 1, 3]
  - !Solution
    bins: [5, 5, 4, 2, 1, 3]
  - !Solution
    bins: [4, 2, 5, 2, 1, 3]
  - !Solution
    bins: [5, 2, 5, 2, 1, 3]
  - !Solution
    bins: [2, 4, 5, 2, 1, 3]
  - !Solution
    bins: [4, 4, 5, 2, 1, 3]
  - !Solution
    bins: [5, 4, 5, 2, 1, 3]
  - !Solution
    bins: [2, 5, 5, 2, 1, 3]
  - !Solution
    bins: [4, 5, 5, 2, 1, 3]
  - !Solution
    bins: [5, 5, 5, 2, 1, 3]
  - !Solution
    bins: [2, 2, 2, 4, 1, 3]
  - !Solution
    bins: [4, 2, 2, 4, 1, 3]
  - !Solution
    bins: [5, 2, 2, 4, 1, 3]
  - !Solution
    bins: [2, 4, 2, 4, 1, 3]
  - !Solution
    bins: [5, 4, 2, 4, 1, 3]
  - !Solution
    bins: [2, 5, 2, 4, 1, 3]
  - !Solution
    bins: [4, 5, 2, 4, 1, 3]
  - !Solution
    bins: [5, 5, 2, 4, 1, 3]
  - !Solution
    bins: [2, 2, 4, 4, 1, 3]
  - !Solution
    bins: [5, 2, 4, 4, 1, 3]
  - !Solution
    bins: [2, 5, 4, 4, 1, 3]
  - !Solution
    bins: [5, 5, 4, 4, 1, 3]
  - !Solution
    bins: [2, 2, 5, 4, 1, 3]
  - !Solution
    bins: [4, 2, 5, 4, 1, 3]
  - !Solution
    bins: [5, 2, 5, 4, 1, 3]
  - !Solution
    bins: [2, 4, 5, 4, 1, 3]
  - !Solution
    bins: [5, 4, 5, 4, 1, 3]
  - !Solution
    bins: [2, 5, 5, 4, 1, 3]
  - !Solution
    bins: [4, 5, 5, 4, 1, 3]
  - !Solution
    bins: [5, 5, 5, 4, 1, 3]
  - !Solution
    bins: [2, 2, 2, 5, 1, 3]
  - !Solution
    bins: [4, 2, 2, 5, 1, 3]
  - !Solution
    bins: [5, 2, 2, 5, 1, 3]
  - !Solution
    bins: [2, 4, 2, 5, 1, 3]
  - !Solution
    bins: [4, 4, 2, 5, 1, 3]
  - !Solution
    bins: [5, 4, 2, 5, 1, 3]
  - !Solution
    bins: [2, 5, 2, 5, 1, 3]
  - !Solution
    bins: [4, 5, 2, 5, 1, 3]
  - !Solution
    bins: [2, 2, 4, 5, 1, 3]
  - !Solution
    bins: [4, 2, 4, 5, 1, 3]
  - !Solution
    bins: [5, 2, 4, 5, 1, 3]
  - !Solution
    bins: [2, 4, 4, 5, 1, 3]
  - !Solution
    bins: [4, 4, 4, 5, 1, 3]
  - !Solution
    bins: [5, 4, 4, 5, 1, 3]
  - !Solution
    bins: [2, 5, 4, 5, 1, 3]
  - !Solution
    bins: [4, 5, 4, 5, 1, 3]
  - !Solution
    bins: [2, 2, 5, 5, 1, 3]
  - !Solution
    bins: [4, 2, 5, 5, 1, 3]
  - !Solution
    bins: [2, 4, 5, 5, 1, 3]
  - !Solution
    bins: [4, 4, 5, 5, 1, 3]
  - !Solution
    bins: [4, 4, 1, 1, 2, 3]
  - !Solution
    bins: [5, 4, 1, 1, 2, 3]
  - !Solution
    bins: [4, 5, 1, 1, 2, 3]
  - !Solution
    bins: [5, 5, 1, 1, 2, 3]
  - !Solution
    bins: [4, 1, 4, 1, 2, 3]
  - !Solution
    bins: [5, 1, 4, 1, 2, 3]
  - !Solution
    bins: [1, 4, 4, 1, 2, 3]
  - !Solution
    bins: [4, 4, 4, 1, 2, 3]
  - !Solution
    bins: [5, 4, 4, 1, 2, 3]
  - !Solution
    bins: [1, 5, 4, 1, 2, 3]
  - !Solution
    bins: [4, 5, 4, 1, 2, 3]
  - !Solution
    bins: [5, 5, 4, 1, 2, 3]
  - !Solution
    bins: [4, 1, 5, 1, 2, 3]
  - !Solution
    bins: [5, 1, 5, 1, 2, 3]
  - !Solution
    bins: [1, 4, 5, 1, 2, 3]
  - !Solution
    bins: [4, 4, 5, 1, 2, 3]
  - !Solution
    bins: [5, 4, 5, 1, 2, 3]
  - !Solution
    bins: [1, 5, 5, 1, 2, 3]
  - !Solution
    bins: [4, 5, 5, 1, 2, 3]
  - !Solution
    bins: [5, 5, 5, 1, 2, 3]
  - !Solution
    bins: [1, 1, 1, 4, 2, 3]
  - !Solution
    bins: [4, 1, 1, 4, 2, 3]
  - !Solution
    bins: [5, 1, 1, 4, 2, 3]
  - !Solution
    bins: [1, 4, 1, 4, 2, 3]
  - !Solution
    bins: [5, 4, 1, 4, 2, 3]
  - !Solution
    bins: [1, 5, 1, 4, 2, 3]
  - !Solution
    bins: [4, 5, 1, 4, 2, 3]
  - !Solution
    bins: [5, 5, 1, 4, 2, 3]
  - !Solution
    bins: [1, 1, 4, 4, 2, 3]
  - !Solution
    bins: [5, 1, 4, 4, 2, 3]
  - !Solution
    bins: [1, 5, 4, 4, 2, 3]
  - !Solution
    bins: [5, 5, 4, 4, 2, 3]
  - !Solution
    bins: [1, 1, 5, 4, 2, 3]
  - !Solution
    bins: [4, 1, 5, 4, 2, 3]
  - !Solution
    bins: [5, 1, 5, 4, 2, 3]
  - !Solution
    bins: [1, 4, 5, 4, 2, 3]
  - !Solution
    bins: [5, 4, 5, 4, 2, 3]
  - !Solution
    bins: [1, 5, 5, 4, 2, 3]
  - !Solution
    bins: [4, 5, 5, 4, 2, 3]
  - !Solution
    bins: [5, 5, 5, 4, 2, 3]
  - !Solution
    bins: [1, 1, 1, 5, 2, 3]
  - !Solution
    bins: [4, 1, 1, 5, 2, 3]
  - !Solution
    bins: [5, 1, 1, 5, 2, 3]
  - !Solution
    bins: [1, 4, 1, 5, 2, 3]
  - !Solution
    bins: [4, 4, 1, 5, 2, 3]
  - !Solution
    bins: [5, 4, 1, 5, 2, 3]
  - !Solution
    bins: [1, 5, 1, 5, 2, 3]
  - !Solution
    bins: [4, 5, 1, 5, 2, 3]
  - !Solution
    bins: [1, 1, 4, 5, 2, 3]
  - !Solution
    bins: [4, 1, 4, 5, 2, 3]
  - !Solution
    bins: [5, 1, 4, 5, 2, 3]
  - !Solution
    bins: [1, 4, 4, 5, 2, 3]
  - !Solution
    bins: [4, 4, 4, 5, 2, 3]
  - !Solution
    bins: [5, 4, 4, 5, 2, 3]
  - !Solution
    bins: [1, 5, 4, 5, 2, 3]
  - !Solution
    bins: [4, 5, 4, 5, 2, 3]
  - !Solution
    bins: [1, 1, 5, 5, 2, 3]
  - !Solution
    bins: [4, 1, 5, 5, 2, 3]
  - !Solution
    bins: [1, 4, 5, 5, 2, 3]
  - !Solution
    bins: [4, 4, 5, 5, 2, 3]
  - !Solution
    bins: [2, 2, 1, 1, 4, 3]
  - !Solution
    bins: [5, 2, 1, 1, 4, 3]
  - !Solution
    bins: [2, 5, 1, 1, 4, 3]
  - !Solution
    bins: [5, 5, 1, 1, 4, 3]
  - !Solution
    bins: [2, 1, 2, 1, 4, 3]
  - !Solution
    bins: [5, 1, 2, 1, 4, 3]
  - !Solution
    bins: [1, 2, 2, 1, 4, 3]
  - !Solution
    bins: [2, 2, 2, 1, 4, 3]
  - !Solution
    bins: [5, 2, 2, 1, 4, 3]
  - !Solution
    bins: [1, 5, 2, 1, 4, 3]
  - !Solution
    bins: [2, 5, 2, 1, 4, 3]
  - !Solution
    bins: [5, 5, 2, 1, 4, 3]
  - !Solution
    bins: [2, 1, 5, 1, 4, 3]
  - !Solution
    bins: [5, 1, 5, 1, 4, 3]
  - !Solution
    bins: [1, 2, 5, 1, 4, 3]
  - !Solution
    bins: [2, 2, 5, 1, 4, 3]
  - !Solution
    bins: [5, 2, 5, 1, 4, 3]
  - !Solution
    bins: [1, 5, 5, 1, 4, 3]
  - !Solution
    bins: [2, 5, 5, 1, 4, 3]
  - !Solution
    bins: [5, 5, 5, 1, 4, 3]
  - !Solution
    bins: [1, 1, 1, 2, 4, 3]
  - !Solution
    bins: [2, 1, 1, 2, 4, 3]
  - !Solution
    bins: [5, 1, 1, 2, 4, 3]
  - !Solution
    bins: [1, 2, 1, 2, 4, 3]
  - !Solution
    bins: [5, 2, 1, 2, 4, 3]
  - !Solution
    bins: [1, 5, 1, 2, 4, 3]
  - !Solution
    bins: [2, 5, 1, 2, 4, 3]
  - !Solution
    bins: [5, 5, 1, 2, 4, 3]
  - !Solution
    bins: [1, 1, 2, 2, 4, 3]
  - !Solution
    bins: [5, 1, 2, 2, 4, 3]
  - !Solution
    bins: [1, 5, 2, 2, 4, 3]
  - !Solution
    bins: [5, 5, 2, 2, 4, 3]
  - !Solution
    bins: [1, 1, 5, 2, 4, 3]
  - !Solution
    bins: [2, 1, 5, 2, 4, 3]
  - !Solution
    bins: [5, 1, 5, 2, 4, 3]
  - !Solution
    bins: [1, 2, 5, 2, 4, 3]
  - !Solution
    bins: [5, 2, 5, 2, 4, 3]
  - !Solution
    bins: [1, 5, 5, 2, 4, 3]
  - !Solution
    bins: [2, 5, 5, 2, 4, 3]
  - !Solution
    bins: [5, 5, 5, 2, 4, 3]
  - !Solution
    bins: [1, 1, 1, 5, 4, 3]
  - !Solution
    bins: [2, 1, 1, 5, 4, 3]
  - !Solution
    bins: [5, 1, 1, 5, 4, 3]
  - !Solution
    bins: [1, 2, 1, 5, 4, 3]
  - !Solution
    bins: [2, 2, 1, 5, 4, 3]
  - !Solution
    bins: [5, 2, 1, 5, 4, 3]
  - !Solution
    bins: [1, 5, 1, 5, 4, 3]
  - !Solution
    bins: [2, 5, 1, 5, 4, 3]
  - !Solution
    bins: [1, 1, 2, 5, 4, 3]
  - !Solution
    bins: [2, 1, 2, 5, 4, 3]
  - !Solution
    bins: [5, 1, 2, 5, 4, 3]
  - !Solution
    bins: [1, 2, 2, 5, 4, 3]
  - !Solution
    bins: [2, 2, 2, 5, 4, 3]
  - !Solution
    bins: [5, 2, 2, 5, 4, 3]
  - !Solution
    bins: [1, 5, 2, 5, 4, 3]
  - !Solution
    bins: [2, 5, 2, 5, 4, 3]
  - !Solution
    bins: [1, 1, 5, 5, 4, 3]
  - !Solution
    bins: [2, 1, 5, 5, 4, 3]
  - !Solution
    bins: [1, 2, 5, 5, 4, 3]
  - !Solution
    bins: [2, 2, 5, 5, 4, 3]
  - !Solution
    bins: [2, 2, 1, 1, 5, 3]
  - !Solution
    bins: [4, 2, 1, 1, 5, 3]
  - !Solution
    bins: [2, 4, 1, 1, 5, 3]
  - !Solution
    bins: [4, 4, 1, 1, 5, 3]
  - !Solution
    bins: [2, 1, 2, 1, 5, 3]
  - !Solution
    bins: [4, 1, 2, 1, 5, 3]
  - !Solution
    bins: [1, 2, 2, 1, 5, 3]
  - !Solution
    bins: [2, 2, 2, 1, 5, 3]
  - !Solution
    bins: [4, 2, 2, 1, 5, 3]
  - !Solution
    bins: [1, 4, 2, 1, 5, 3]
  - !Solution
    bins: [2, 4, 2, 1, 5, 3]
  - !Solution
    bins: [4, 4, 2, 1, 5, 3]
  - !Solution
    bins: [2, 1, 4, 1, 5, 3]
  - !Solution
    bins: [4, 1, 4, 1, 5, 3]
  - !Solution
    bins: [1, 2, 4, 1, 5, 3]
  - !Solution
    bins: [2, 2, 4, 1, 5, 3]
  - !Solution
    bins: [4, 2, 4, 1, 5, 3]
  - !Solution
    bins: [1, 4, 4, 1, 5, 3]
  - !Solution
    bins: [2, 4, 4, 1, 5, 3]
  - !Solution
    bins: [4, 4, 4, 1, 5, 3]
  - !Solution
    bins: [1, 1, 1, 2, 5, 3]
  - !Solution
    bins: [2, 1, 1, 2, 5, 3]
  - !Solution
    bins: [4, 1, 1, 2, 5, 3]
  - !Solution
    bins: [1, 2, 1, 2, 5, 3]
  - !Solution
    bins: [4, 2, 1, 2, 5, 3]
  - !Solution
    bins: [1, 4, 1, 2, 5, 3]
  - !Solution
    bins: [2, 4, 1, 2, 5, 3]
  - !Solution
    bins: [4, 4, 1, 2, 5, 3]
  - !Solution
    bins: [1, 1, 2, 2, 5, 3]
  - !Solution
    bins: [4, 1, 2, 2, 5, 3]
  - !Solution
    bins: [1, 4, 2, 2, 5, 3]
  - !Solution
    bins: [4, 4, 2, 2, 5, 3]
  - !Solution
    bins: [1, 1, 4, 2, 5, 3]
  - !Solution
    bins: [2, 1, 4, 2, 5, 3]
  - !Solution
    bins: [4, 1, 4, 2, 5, 3]
  - !Solution
    bins: [1, 2, 4, 2, 5, 3]
  - !Solution
    bins: [4, 2, 4, 2, 5, 3]
  - !Solution
    bins: [1, 4, 4, 2, 5, 3]
  - !Solution
    bins: [2, 4, 4, 2, 5, 3]
  - !Solution
    bins: [4, 4, 4, 2, 5, 3]
  - !Solution
    bins: [1, 1, 1, 4, 5, 3]
  - !Solution
    bins: [2, 1, 1, 4, 5, 3]
  - !Solution
    bins: [4, 1, 1, 4, 5, 3]
  - !Solution
    bins: [1, 2, 1, 4, 5, 3]
  - !Solution
    bins: [2, 2, 1, 4, 5, 3]
  - !Solution
    bins: [4, 2, 1, 4, 5, 3]
  - !Solution
    bins: [1, 4, 1, 4, 5, 3]
  - !Solution
    bins: [2, 4, 1, 4, 5, 3]
  - !Solution
    bins: [1, 1, 2, 4, 5, 3]
  - !Solution
    bins: [2, 1, 2, 4, 5, 3]
  - !Solution
    bins: [4, 1, 2, 4, 5, 3]
  - !Solution
    bins: [1, 2, 2, 4, 5, 3]
  - !Solution
    bins: [2, 2, 2, 4, 5, 3]
  - !Solution
    bins: [4, 2, 2, 4, 5, 3]
  - !Solution
    bins: [1, 4, 2, 4, 5, 3]
  - !Solution
    bins: [2, 4, 2, 4, 5, 3]
  - !Solution
    bins: [1, 1, 4, 4, 5, 3]
  - !Solution
    bins: [2, 1, 4, 4, 5, 3]
  - !Solution
    bins: [1, 2, 4, 4, 5, 3]
  - !Solution
    bins: [2, 2, 4, 4, 5, 3]
  - !Solution
    bins: [3, 3, 2, 2, 1, 4]
  - !Solution
    bins: [5, 3, 2, 2, 1, 4]
  - !Solution
    bins: [3, 5, 2, 2, 1, 4]
  - !Solution
    bins: [5, 5, 2, 2, 1, 4]
  - !Solution
    bins: [3, 2, 3, 2, 1, 4]
  - !Solution
    bins: [5, 2, 3, 2, 1, 4]
  - !Solution
    bins: [2, 3, 3, 2, 1, 4]
  - !Solution
    bins: [3, 3, 3, 2, 1, 4]
  - !Solution
    bins: [5, 3, 3, 2, 1, 4]
  - !Solution
    bins: [2, 5, 3, 2, 1, 4]
  - !Solution
    bins: [3, 5, 3, 2, 1, 4]
  - !Solution
    bins: [5, 5, 3, 2, 1, 4]
  - !Solution
    bins: [3, 2, 5, 2, 1, 4]
  - !Solution
    bins: [5, 2, 5, 2, 1, 4]
  - !Solution
    bins: [2, 3, 5, 2, 1, 4]
  - !Solution
    bins: [3, 3, 5, 2, 1, 4]
  - !Solution
    bins: [5, 3, 5, 2, 1, 4]
  - !Solution
    bins: [2, 5, 5, 2, 1, 4]
  - !Solution
    bins: [3, 5, 5, 2, 1, 4]
  - !Solution
    bins: [5, 5, 5, 2, 1, 4]
  - !Solution
    bins: [2, 2, 2, 3, 1, 4]
  - !Solution
    bins: [3, 2, 2, 3, 1, 4]
  - !Solution
    bins: [5, 2, 2, 3, 1, 4]
  - !Solution
    bins: [2, 3, 2, 3, 1, 4]
  - !Solution
    bins: [5, 3, 2, 3, 1, 4]
  - !Solution
    bins: [2, 5, 2, 3, 1, 4]
  - !Solution
    bins: [3, 5, 2, 3, 1, 4]
  - !Solution
    bins: [5, 5, 2, 3, 1, 4]
  - !Solution
    bins: [2, 2, 3, 3, 1, 4]
  - !Solution
    bins: [5, 2, 3, 3, 1, 4]
  - !Solution
    bins: [2, 5, 3, 3, 1, 4]
  - !Solution
    bins: [5, 5, 3, 3, 1, 4]
  - !Solution
    bins: [2, 2, 5, 3, 1, 4]
  - !Solution
    bins: [3, 2, 5, 3, 1, 4]
  - !Solution
    bins: [5, 2, 5, 3, 1, 4]
  - !Solution
    bins: [2, 3, 5, 3, 1, 4]
  - !Solution
    bins: [5, 3, 5, 3, 1, 4]
  - !Solution
    bins: [2, 5, 5, 3, 1, 4]
  - !Solution
    bins: [3, 5, 5, 3, 1, 4]
  - !Solution
    bins: [5, 5, 5, 3, 1, 4]
  - !Solution
    bins: [2, 2, 2, 5, 1, 4]
  - !Solution
    bins: [3, 2, 2, 5, 1, 4]
  - !Solution
    bins: [5, 2, 2, 5, 1, 4]
  - !Solution
    bins: [2, 3, 2, 5, 1, 4]
  - !Solution
    bins: [3, 3, 2, 5, 1, 4]
  - !Solution
    bins: [5, 3, 2, 5, 1, 4]
  - !Solution
    bins: [2, 5, 2, 5, 1, 4]
  - !Solution
    bins: [3, 5, 2, 5, 1, 4]
  - !Solution
    bins: [2, 2, 3, 5, 1, 4]
  - !Solution
    bins: [3, 2, 3, 5, 1, 4]
  - !Solution
    bins: [5, 2, 3, 5, 1, 4]
  - !Solution
    bins: [2, 3, 3, 5, 1, 4]
  - !Solution
    bins: [3, 3, 3, 5, 1, 4]
  - !Solution
    bins: [5, 3, 3, 5, 1, 4]
  - !Solution
    bins: [2, 5, 3, 5, 1, 4]
  - !Solution
    bins: [3, 5, 3, 5, 1, 4]
  - !Solution
    bins: [2, 2, 5, 5, 1, 4]
  - !Solution
    bins: [3, 2, 5, 5, 1, 4]
  - !Solution
    bins: [2, 3, 5, 5, 1, 4]
  - !Solution
    bins: [3, 3, 5, 5, 1, 4]
  - !Solution
    bins: [3, 3, 1, 1, 2, 4]
  - !Solution
    bins: [5, 3, 1, 1, 2, 4]
  - !Solution
    bins: [3, 5, 1, 1, 2, 4]
  - !Solution
    bins: [5, 5, 1, 1, 2, 4]
  - !Solution
    bins: [3, 1, 3, 1, 2, 4]
  - !Solution
    bins: [5, 1, 3, 1, 2, 4]
  - !Solution
    bins: [1, 3, 3, 1, 2, 4]
  - !Solution
    bins: [3, 3, 3, 1, 2, 4]
  - !Solution
    bins: [5, 3, 3, 1, 2, 4]
  - !Solution
    bins: [1, 5, 3, 1, 2, 4]
  - !Solution
    bins: [3, 5, 3, 1, 2, 4]
  - !Solution
    bins: [5, 5, 3, 1, 2, 4]
  - !Solution
    bins: [3, 1, 5, 1, 2, 4]
  - !Solution
    bins: [5, 1, 5, 1, 2, 4]
  - !Solution
    bins: [1, 3, 5, 1, 2, 4]
  - !Solution
    bins: [3, 3, 5, 1, 2, 4]
  - !Solution
    bins: [5, 3, 5, 1, 2, 4]
  - !Solution
    bins: [1, 5, 5, 1, 2, 4]
  - !Solution
    bins: [3, 5, 5, 1, 2, 4]
  - !Solution
    bins: [5, 5, 5, 1, 2, 4]
  - !Solution
    bins: [1, 1, 1, 3, 2, 4]
  - !Solution
    bins: [3, 1, 1, 3, 2, 4]
  - !Solution
    bins: [5, 1, 1, 3, 2, 4]
  - !Solution
    bins: [1, 3, 1, 3, 2, 4]
  - !Solution
    bins: [5, 3, 1, 3, 2, 4]
  - !Solution
    bins: [1, 5, 1, 3, 2, 4]
  - !Solution
    bins: [3, 5, 1, 3, 2, 4]
  - !Solution
    bins: [5, 5, 1, 3, 2, 4]
  - !Solution
    bins: [1, 1, 3, 3, 2, 4]
  - !Solution
    bins: [5, 1, 3, 3, 2, 4]
  - !Solution
    bins: [1, 5, 3, 3, 2, 4]
  - !Solution
    bins: [5, 5, 3, 3, 2, 4]
  - !Solution
    bins: [1, 1, 5, 3, 2, 4]
  - !Solution
    bins: [3, 1, 5, 3, 2, 4]
  - !Solution
    bins: [5, 1, 5, 3, 2, 4]
  - !Solution
    bins: [1, 3, 5, 3, 2, 4]
  - !Solution
    bins: [5, 3, 5, 3, 2, 4]
  - !Solution
    bins: [1, 5, 5, 3, 2, 4]
  - !Solution
    bins: [3, 5, 5, 3, 2, 4]
  - !Solution
    bins: [5, 5, 5, 3, 2, 4]
  - !Solution
    bins: [1, 1, 1, 5, 2, 4]
  - !Solution
    bins: [3, 1, 1, 5, 2, 4]
  - !Solution
    bins: [5, 1, 1, 5, 2, 4]
  - !Solution
    bins: [1, 3, 1, 5, 2, 4]
  - !Solution
    bins: [3, 3, 1, 5, 2, 4]
  - !Solution
    bins: [5, 3, 1, 5, 2, 4]
  - !Solution
    bins: [1, 5, 1, 5, 2, 4]
  - !Solution
    bins: [3, 5, 1, 5, 2, 4]
  - !Solution
    bins: [1, 1, 3, 5, 2, 4]
  - !Solution
    bins: [3, 1, 3, 5, 2, 4]
  - !Solution
    bins: [5, 1, 3, 5, 2, 4]
  - !Solution
    bins: [1, 3, 3, 5, 2, 4]
  - !Solution
    bins: [3, 3, 3, 5, 2, 4]
  - !Solution
    bins: [5, 3, 3, 5, 2, 4]
  - !Solution
    bins: [1, 5, 3, 5, 2, 4]
  - !Solution
    bins: [3, 5, 3, 5, 2, 4]
  - !Solution
    bins: [1, 1, 5, 5, 2, 4]
  - !Solution
    bins: [3, 1, 5, 5, 2, 4]
  - !Solution
    bins: [1, 3, 5, 5, 2, 4]
  - !Solution
    bins: [3, 3, 5, 5, 2, 4]
  - !Solution
    bins: [2, 2, 1, 1, 3, 4]
  - !Solution
    bins: [5, 2, 1, 1, 3, 4]
  - !Solution
    bins: [2, 5, 1, 1, 3, 4]
  - !Solution
    bins: [5, 5, 1, 1, 3, 4]
  - !Solution
    bins: [2, 1, 2, 1, 3, 4]
  - !Solution
    bins: [5, 1, 2, 1, 3, 4]
  - !Solution
    bins: [1, 2, 2, 1, 3, 4]
  - !Solution
    bins: [2, 2, 2, 1, 3, 4]
  - !Solution
    bins: [5, 2, 2, 1, 3, 4]
  - !Solution
    bins: [1, 5, 2, 1, 3, 4]
  - !Solution
    bins: [2, 5, 2, 1, 3, 4]
  - !Solution
    bins: [5, 5, 2, 1, 3, 4]
  - !Solution
    bins: [2, 1, 5, 1, 3, 4]
  - !Solution
    bins: [5, 1, 5, 1, 3, 4]
  - !Solution
    bins: [1, 2, 5, 1, 3, 4]
  - !Solution
    bins: [2, 2, 5, 1, 3, 4]
  - !Solution
    bins: [5, 2, 5, 1, 3, 4]
  - !Solution
    bins: [1, 5, 5, 1, 3, 4]
  - !Solution
    bins: [2, 5, 5, 1, 3, 4]
  - !Solution
    bins: [5, 5, 5, 1, 3, 4]
  - !Solution
    bins: [1, 1, 1, 2, 3, 4]
  - !Solution
    bins: [2, 1, 1, 2, 3, 4]
  - !Solution
    bins: [5, 1, 1, 2, 3, 4]
  - !Solution
    bins: [1, 2, 1, 2, 3, 4]
  - !Solution
    bins: [5, 2, 1, 2, 3, 4]
  - !Solution
    bins: [1, 5, 1, 2, 3, 4]
  - !Solution
    bins: [2, 5, 1, 2, 3, 4]
  - !Solution
    bins: [5, 5, 1, 2, 3, 4]
  - !Solution
    bins: [1, 1, 2, 2, 3, 4]
  - !Solution
    bins: [5, 1, 2, 2, 3, 4]
  - !Solution
    bins: [1, 5, 2, 2, 3, 4]
  - !Solution
    bins: [5, 5, 2, 2, 3, 4]
  - !Solution
    bins: [1, 1, 5, 2, 3, 4]
  - !Solution
    bins: [2, 1, 5, 2, 3, 4]
  - !Solution
    bins: [5, 1, 5, 2, 3, 4]
  - !Solution
    bins: [1, 2, 5, 2, 3, 4]
  - !Solution
    bins: [5, 2, 5, 2, 3, 4]
  - !Solution
    bins: [1, 5, 5, 2, 3, 4]
  - !Solution
    bins: [2, 5, 5, 2, 3, 4]
  - !Solution
    bins: [5, 5, 5, 2, 3, 4]
  - !Solution
    bins: [1, 1, 1, 5, 3, 4]
  - !Solution
    bins: [2, 1, 1, 5, 3, 4]
  - !Solution
    bins: [5, 1, 1, 5, 3, 4]
  - !Solution
    bins: [1, 2, 1, 5, 3, 4]
  - !Solution
    bins: [2, 2, 1, 5, 3, 4]
  - !Solution
    bins: [5, 2, 1, 5, 3, 4]
  - !Solution
    bins: [1, 5, 1, 5, 3, 4]
  - !Solution
    bins: [2, 5, 1, 5, 3, 4]
  - !Solution
    bins: [1, 1, 2, 5, 3, 4]
  - !Solution
    bins: [2, 1, 2, 5, 3, 4]
  - !Solution
    bins: [5, 1, 2, 5, 3, 4]
  - !Solution
    bins: [1, 2, 2, 5, 3, 4]
  - !Solution
    bins: [2, 2, 2, 5, 3, 4]
  - !Solution
    bins: [5, 2, 2, 5, 3, 4]
  - !Solution
    bins: [1, 5, 2, 5, 3, 4]
  - !Solution
    bins: [2, 5, 2, 5, 3, 4]
  - !Solution
    bins: [1, 1, 5, 5, 3, 4]
  - !Solution
    bins: [2, 1, 5, 5, 3, 4]
  - !Solution
    bins: [1, 2, 5, 5, 3, 4]
  - !Solution
    bins: [2, 2, 5, 5, 3, 4]
  - !Solution
    bins: [2, 2, 1, 1, 5, 4]
  - !Solution
    bins: [3, 2, 1, 1, 5, 4]
  - !Solution
    bins: [2, 3, 1, 1, 5, 4]
  - !Solution
    bins: [3, 3, 1, 1, 5, 4]
  - !Solution
    bins: [2, 1, 2, 1, 5, 4]
  - !Solution
    bins: [3, 1, 2, 1, 5, 4]
  - !Solution
    bins: [1, 2, 2, 1, 5, 4]
  - !Solution
    bins: [2, 2, 2, 1, 5, 4]
  - !Solution
    bins: [3, 2, 2, 1, 5, 4]
  - !Solution
    bins: [1, 3, 2, 1, 5, 4]
  - !Solution
    bins: [2, 3, 2, 1, 5, 4]
  - !Solution
    bins: [3, 3, 2, 1, 5, 4]
  - !Solution
    bins: [2, 1, 3, 1, 5, 4]
  - !Solution
    bins: [3, 1, 3, 1, 5, 4]
  - !Solution
    bins: [1, 2, 3, 1, 5, 4]
  - !Solution
    bins: [2, 2, 3, 1, 5, 4]
  - !Solution
    bins: [3, 2, 3, 1, 5, 4]
  - !Solution
    bins: [1, 3, 3, 1, 5, 4]
  - !Solution
    bins: [2, 3, 3, 1, 5, 4]
  - !Solution
    bins: [3, 3, 3, 1, 5, 4]
  - !Solution
    bins: [1, 1, 1, 2, 5, 4]
  - !Solution
    bins: [2, 1, 1, 2, 5, 4]
  - !Solution
    bins: [3, 1, 1, 2, 5, 4]
  - !Solution
    bins: [1, 2, 1, 2, 5, 4]
  - !Solution
    bins: [3, 2, 1, 2, 5, 4]
  - !Solution
    bins: [1, 3, 1, 2, 5, 4]
  - !Solution
    bins: [2, 3, 1, 2, 5, 4]
  - !Solution
    bins: [3, 3, 1, 2, 5, 4]
  - !Solution
    bins: [1, 1, 2, 2, 5, 4]
  - !Solution
    bins: [3, 1, 2, 2, 5, 4]
  - !Solution
    bins: [1, 3, 2, 2, 5, 4]
  - !Solution
    bins: [3, 3, 2, 2, 5, 4]
  - !Solution
    bins: [1, 1, 3, 2, 5, 4]
  - !Solution
    bins: [2, 1, 3, 2, 5, 4]
  - !Solution
    bins: [3, 1, 3, 2, 5, 4]
  - !Solution
    bins: [1, 2, 3, 2, 5, 4]
  - !Solution
    bins: [3, 2, 3, 2, 5, 4]
  - !Solution
    bins: [1, 3, 3, 2, 5, 4]
  - !Solution
    bins: [2, 3, 3, 2, 5, 4]
  - !Solution
    bins: [3, 3, 3, 2, 5, 4]
  - !Solution
    bins: [1, 1, 1, 3, 5, 4]
  - !Solution
    bins: [2, 1, 1, 3, 5, 4]
  - !Solution
    bins: [3, 1, 1, 3, 5, 4]
  - !Solution
    bins: [1, 2, 1, 3, 5, 4]
  - !Solution
    bins: [2, 2, 1, 3, 5, 4]
  - !Solution
    bins: [3, 2, 1, 3, 5, 4]
  - !Solution
    bins: [1, 3, 1, 3, 5, 4]
  - !Solution
    bins: [2, 3, 1, 3, 5, 4]
  - !Solution
    bins: [1, 1, 2, 3, 5, 4]
  - !Solution
    bins: [2, 1, 2, 3, 5, 4]
  - !Solution
    bins: [3, 1, 2, 3, 5, 4]
  - !Solution
    bins: [1, 2, 2, 3, 5, 4]
  - !Solution
    bins: [2, 2, 2, 3, 5, 4]
  - !Solution
    bins: [3, 2, 2, 3, 5, 4]
  - !Solution
    bins: [1, 3, 2, 3, 5, 4]
  - !Solution
    bins: [2, 3, 2, 3, 5, 4]
  - !Solution
    bins: [1, 1, 3, 3, 5, 4]
  - !Solution
    bins: [2, 1, 3, 3, 5, 4]
  - !Solution
    bins: [1, 2, 3, 3, 5, 4]
  - !Solution
    bins: [2, 2, 3, 3, 5, 4]
  - !Solution
    bins: [3, 3, 2, 2, 1, 5]
  - !Solution
    bins: [4, 3, 2, 2, 1, 5]
  - !Solution
    bins: [3, 4, 2, 2, 1, 5]
  - !Solution
    bins: [4, 4, 2, 2, 1, 5]
  - !Solution
    bins: [3, 2, 3, 2, 1, 5]
  - !Solution
    bins: [4, 2, 3, 2, 1, 5]
  - !Solution
    bins: [2, 3, 3, 2, 1, 5]
  - !Solution
    bins: [3, 3, 3, 2, 1, 5]
  - !Solution
    bins: [4, 3, 3, 2, 1, 5]
  - !Solution
    bins: [2, 4, 3, 2, 1, 5]
  - !Solution
    bins: [3, 4, 3, 2, 1, 5]
  - !Solution
    bins: [4, 4, 3, 2, 1, 5]
  - !Solution
    bins: [3, 2, 4, 2, 1, 5]
  - !Solution
    bins: [4, 2, 4, 2, 1, 5]
  - !Solution
    bins: [2, 3, 4, 2, 1, 5]
  - !Solution
    bins: [3, 3, 4, 2, 1, 5]
  - !Solution
    bins: [4, 3, 4, 2, 1, 5]
  - !Solution
    bins: [2, 4, 4, 2, 1, 5]
  - !Solution
    bins: [3, 4, 4, 2, 1, 5]
  - !Solution
    bins: [4, 4, 4, 2, 1, 5]
  - !Solution
    bins: [2, 2, 2, 3, 1, 5]
  - !Solution
    bins: [3, 2, 2, 3, 1, 5]
  - !Solution
    bins: [4, 2, 2, 3, 1, 5]
  - !Solution
    bins: [2, 3, 2, 3, 1, 5]
  - !Solution
    bins: [4, 3, 2, 3, 1, 5]
  - !Solution
    bins: [2, 4, 2, 3, 1, 5]
  - !Solution
    bins: [3, 4, 2, 3, 1, 5]
  - !Solution
    bins: [4, 4, 2, 3, 1, 5]
  - !Solution
    bins: [2, 2, 3, 3, 1, 5]
  - !Solution
    bins: [4, 2, 3, 3, 1, 5]
  - !Solution
    bins: [2, 4, 3, 3, 1, 5]
  - !Solution
    bins: [4, 4, 3, 3, 1, 5]
  - !Solution
    bins: [2, 2, 4, 3, 1, 5]
  - !Solution
    bins: [3, 2, 4, 3, 1, 5]
  - !Solution
    bins: [4, 2, 4, 3, 1, 5]
  - !Solution
    bins: [2, 3, 4, 3, 1, 5]
  - !Solution
    bins: [4, 3, 4, 3, 1, 5]
  - !Solution
    bins: [2, 4, 4, 3, 1, 5]
  - !Solution
    bins: [3, 4, 4, 3, 1, 5]
  - !Solution
    bins: [4, 4, 4, 3, 1, 5]
  - !Solution
    bins: [2, 2, 2, 4, 1, 5]
  - !Solution
    bins: [3, 2, 2, 4, 1, 5]
  - !Solution
    bins: [4, 2, 2, 4, 1, 5]
  - !Solution
    bins: [2, 3, 2, 4, 1, 5]
  - !Solution
    bins: [3, 3, 2, 4, 1, 5]
  - !Solution
    bins: [4, 3, 2, 4, 1, 5]
  - !Solution
    bins: [2, 4, 2, 4, 1, 5]
  - !Solution
    bins: [3, 4, 2, 4, 1, 5]
  - !Solution
    bins: [2, 2, 3, 4, 1, 5]
  - !Solution
    bins: [3, 2, 3, 4, 1, 5]
  - !Solution
    bins: [4, 2, 3, 4, 1, 5]
  - !Solution
    bins: [2, 3, 3, 4, 1, 5]
  - !Solution
    bins: [3, 3, 3, 4, 1, 5]
  - !Solution
    bins: [4, 3, 3, 4, 1, 5]
  - !Solution
    bins: [2, 4, 3, 4, 1, 5]
  - !Solution
    bins: [3, 4, 3, 4, 1, 5]
  - !Solution
    bins: [2, 2, 4, 4, 1, 5]
  - !Solution
    bins: [3, 2, 4, 4, 1, 5]
  - !Solution
    bins: [2, 3, 4, 4, 1, 5]
  - !Solution
    bins: [3, 3, 4, 4, 1, 5]
  - !Solution
    bins: [3, 3, 1, 1, 2, 5]
  - !Solution
    bins: [4, 3, 1, 1, 2, 5]
  - !Solution
    bins: [3, 4, 1, 1, 2, 5]
  - !Solution
    bins: [4, 4, 1, 1, 2, 5]
  - !Solution
    bins: [3, 1, 3, 1, 2, 5]
  - !Solution
    bins: [4, 1, 3, 1, 2, 5]
  - !Solution
    bins: [1, 3, 3, 1, 2, 5]
  - !Solution
    bins: [3, 3, 3, 1, 2, 5]
  - !Solution
    bins: [4, 3, 3, 1, 2, 5]
  - !Solution
    bins: [1, 4, 3, 1, 2, 5]
  - !Solution
    bins: [3, 4, 3, 1, 2, 5]
  - !Solution
    bins: [4, 4, 3, 1, 2, 5]
  - !Solution
    bins: [3, 1, 4, 1, 2, 5]
  - !Solution
    bins: [4, 1, 4, 1, 2, 5]
  - !Solution
    bins: [1, 3, 4, 1, 2, 5]
  - !Solution
    bins: [3, 3, 4, 1, 2, 5]
  - !Solution
    bins: [4, 3, 4, 1, 2, 5]
  - !Solution
    bins: [1, 4, 4, 1, 2, 5]
  - !Solution
    bins: [3, 4, 4, 1, 2, 5]
  - !Solution
    bins: [4, 4, 4, 1, 2, 5]
  - !Solution
    bins: [1, 1, 1, 3, 2, 5]
  - !Solution
    bins: [3, 1, 1, 3, 2, 5]
  - !Solution
    bins: [4, 1, 1, 3, 2, 5]
  - !Solution
    bins: [1, 3, 1, 3, 2, 5]
  - !Solution
    bins: [4, 3, 1, 3, 2, 5]
  - !Solution
    bins: [1, 4, 1, 3, 2, 5]
  - !Solution
    bins: [3, 4, 1, 3, 2, 5]
  - !Solution
    bins: [4, 4, 1, 3, 2, 5]
  - !Solution
    bins: [1, 1, 3, 3, 2, 5]
  - !Solution
    bins: [4, 1, 3, 3, 2, 5]
  - !Solution
    bins: [1, 4, 3, 3, 2, 5]
  - !Solution
    bins: [4, 4, 3, 3, 2, 5]
  - !Solution
    bins: [1, 1, 4, 3, 2, 5]
  - !Solution
    bins: [3, 1, 4, 3, 2, 5]
  - !Solution
    bins: [4, 1, 4, 3, 2, 5]
  - !Solution
    bins: [1, 3, 4, 3, 2, 5]
  - !Solution
    bins: [4, 3, 4, 3, 2, 5]
  - !Solution
    bins: [1, 4, 4, 3, 2, 5]
  - !Solution
    bins: [3, 4, 4, 3, 2, 5]
  - !Solution
    bins: [4, 4, 4, 3, 2, 5]
  - !Solution
    bins: [1, 1, 1, 4, 2, 5]
  - !Solution
    bins: [3, 1, 1, 4, 2, 5]
  - !Solution
    bins: [4, 1, 1, 4, 2, 5]
  - !Solution
    bins: [1, 3, 1, 4, 2, 5]
  - !Solution
    bins: [3, 3, 1, 4, 2, 5]
  - !Solution
    bins: [4, 3, 1, 4, 2, 5]
  - !Solution
    bins: [1, 4, 1, 4, 2, 5]
  - !Solution
    bins: [3, 4, 1, 4, 2, 5]
  - !Solution
    bins: [1, 1, 3, 4, 2, 5]
  - !Solution
    bins: [3, 1, 3, 4, 2, 5]
  - !Solution
    bins: [4, 1, 3, 4, 2, 5]
  - !Solution
    bins: [1, 3, 3, 4, 2, 5]
  - !Solution
    bins: [3, 3, 3, 4, 2, 5]
  - !Solution
    bins: [4, 3, 3, 4, 2, 5]
  - !Solution
    bins: [1, 4, 3, 4, 2, 5]
  - !Solution
    bins: [3, 4, 3, 4, 2, 5]
  - !Solution
    bins: [1, 1, 4, 4, 2, 5]
  - !Solution
    bins: [3, 1, 4, 4, 2, 5]
  - !Solution
    bins: [1, 3, 4, 4, 2, 5]
  - !Solution
    bins: [3, 3, 4, 4, 2, 5]
  - !Solution
    bins: [2, 2, 1, 1, 3, 5]
  - !Solution
    bins: [4, 2, 1, 1, 3, 5]
  - !Solution
    bins: [2, 4, 1, 1, 3, 5]
  - !Solution
    bins: [4, 4, 1, 1, 3, 5]
  - !Solution
    bins: [2, 1, 2, 1, 3, 5]
  - !Solution
    bins: [4, 1, 2, 1, 3, 5]
  - !Solution
    bins: [1, 2, 2, 1, 3, 5]
  - !Solution
    bins: [2, 2, 2, 1, 3, 5]
  - !Solution
    bins: [4, 2, 2, 1, 3, 5]
  - !Solution
    bins: [1, 4, 2, 1, 3, 5]
  - !Solution
    bins: [2, 4, 2, 1, 3, 5]
  - !Solution
    bins: [4, 4, 2, 1, 3, 5]
  - !Solution
    bins: [2, 1, 4, 1, 3, 5]
  - !Solution
    bins: [4, 1, 4, 1, 3, 5]
  - !Solution
    bins: [1, 2, 4, 1, 3, 5]
  - !Solution
    bins: [2, 2, 4, 1, 3, 5]
  - !Solution
    bins: [4, 2, 4, 1, 3, 5]
  - !Solution
    bins: [1, 4, 4, 1, 3, 5]
  - !Solution
    bins: [2, 4, 4, 1, 3, 5]
  - !Solution
    bins: [4, 4, 4, 1, 3, 5]
  - !Solution
    bins: [1, 1, 1, 2, 3, 5]
  - !Solution
    bins: [2, 1, 1, 2, 3, 5]
  - !Solution
    bins: [4, 1, 1, 2, 3, 5]
  - !Solution
    bins: [1, 2, 1, 2, 3, 5]
  - !Solution
    bins: [4, 2, 1, 2, 3, 5]
  - !Solution
    bins: [1, 4, 1, 2, 3, 5]
  - !Solution
    bins: [2, 4, 1, 2, 3, 5]
  - !Solution
    bins: [4, 4, 1, 2, 3, 5]
  - !Solution
    bins: [1, 1, 2, 2, 3, 5]
  - !Solution
    bins: [4, 1, 2, 2, 3, 5]
  - !Solution
    bins: [1, 4, 2, 2, 3, 5]
  - !Solution
    bins: [4, 4, 2, 2, 3, 5]
  - !Solution
    bins: [1, 1, 4, 2, 3, 5]
  - !Solution
    bins: [2, 1, 4, 2, 3, 5]
  - !Solution
    bins: [4, 1, 4, 2, 3, 5]
  - !Solution
    bins: [1, 2, 4, 2, 3, 5]
  - !Solution
    bins: [4, 2, 4, 2, 3, 5]
  - !Solution
    bins: [1, 4, 4, 2, 3, 5]
  - !Solution
    bins: [2, 4, 4, 2, 3, 5]
  - !Solution
    bins: [4, 4, 4, 2, 3, 5]
  - !Solution
    bins: [1, 1, 1, 4, 3, 5]
  - !Solution
    bins: [2, 1, 1, 4, 3, 5]
  - !Solution
    bins: [4, 1, 1, 4, 3, 5]
  - !Solution
    bins: [1, 2, 1, 4, 3, 5]
  - !Solution
    bins: [2, 2, 1, 4, 3, 5]
  - !Solution
    bins: [4, 2, 1, 4, 3, 5]
  - !Solution
    bins: [1, 4, 1, 4, 3, 5]
  - !Solution
    bins: [2, 4, 1, 4, 3, 5]
  - !Solution
    bins: [1, 1, 2, 4, 3, 5]
  - !Solution
    bins: [2, 1, 2, 4, 3, 5]
  - !Solution
    bins: [4, 1, 2, 4, 3, 5]
  - !Solution
    bins: [1, 2, 2, 4, 3, 5]
  - !Solution
    bins: [2, 2, 2, 4, 3, 5]
  - !Solution
    bins: [4, 2, 2, 4, 3, 5]
  - !Solution
    bins: [1, 4, 2, 4, 3, 5]
  - !Solution
    bins: [2, 4, 2, 4, 3, 5]
  - !Solution
    bins: [1, 1, 4, 4, 3, 5]
  - !Solution
    bins: [2, 1, 4, 4, 3, 5]
  - !Solution
    bins: [1, 2, 4, 4, 3, 5]
  - !Solution
    bins: [2, 2, 4, 4, 3, 5]
  - !Solution
    bins: [2, 2, 1, 1, 4, 5]
  - !Solution
    bins: [3, 2, 1, 1, 4, 5]
  - !Solution
    bins: [2, 3, 1, 1, 4, 5]
  - !Solution
    bins: [3, 3, 1, 1, 4, 5]
  - !Solution
    bins: [2, 1, 2, 1, 4, 5]
  - !Solution
    bins: [3, 1, 2, 1, 4, 5]
  - !Solution
    bins: [1, 2, 2, 1, 4, 5]
  - !Solution
    bins: [2, 2, 2, 1, 4, 5]
  - !Solution
    bins: [3, 2, 2, 1, 4, 5]
  - !Solution
    bins: [1, 3, 2, 1, 4, 5]
  - !Solution
    bins: [2, 3, 2, 1, 4, 5]
  - !Solution
    bins: [3, 3, 2, 1, 4, 5]
  - !Solution
    bins: [2, 1, 3, 1, 4, 5]
  - !Solution
    bins: [3, 1, 3, 1, 4, 5]
  - !Solution
    bins: [1, 2, 3, 1, 4, 5]
  - !Solution
    bins: [2, 2, 3, 1, 4, 5]
  - !Solution
    bins: [3, 2, 3, 1, 4, 5]
  - !Solution
    bins: [1, 3, 3, 1, 4, 5]
  - !Solution
    bins: [2, 3, 3, 1, 4, 5]
  - !Solution
    bins: [3, 3, 3, 1, 4, 5]
  - !Solution
    bins: [1, 1, 1, 2, 4, 5]
  - !Solution
    bins: [2, 1, 1, 2, 4, 5]
  - !Solution
    bins: [3, 1, 1, 2, 4, 5]
  - !Solution
    bins: [1, 2, 1, 2, 4, 5]
  - !Solution
    bins: [3, 2, 1, 2, 4, 5]
  - !Solution
    bins: [1, 3, 1, 2, 4, 5]
  - !Solution
    bins: [2, 3, 1, 2, 4, 5]
  - !Solution
    bins: [3, 3, 1, 2, 4, 5]
  - !Solution
    bins: [1, 1, 2, 2, 4, 5]
  - !Solution
    bins: [3, 1, 2, 2, 4, 5]
  - !Solution
    bins: [1, 3, 2, 2, 4, 5]
  - !Solution
    bins: [3, 3, 2, 2, 4, 5]
  - !Solution
    bins: [1, 1, 3, 2, 4, 5]
  - !Solution
    bins: [2, 1, 3, 2, 4, 5]
  - !Solution
    bins: [3, 1, 3, 2, 4, 5]
  - !Solution
    bins: [1, 2, 3, 2, 4, 5]
  - !Solution
    bins: [3, 2, 3, 2, 4, 5]
  - !Solution
    bins: [1, 3, 3, 2, 4, 5]
  - !Solution
    bins: [2, 3, 3, 2, 4, 5]
  - !Solution
    bins: [3, 3, 3, 2, 4, 5]
  - !Solution
    bins: [1, 1, 1, 3, 4, 5]
  - !Solution
    bins: [2, 1, 1, 3, 4, 5]
  - !Solution
    bins: [3, 1, 1, 3, 4, 5]
  - !Solution
    bins: [1, 2, 1, 3, 4, 5]
  - !Solution
    bins: [2, 2, 1, 3, 4, 5]
  - !Solution
    bins: [3, 2, 1, 3, 4, 5]
  - !Solution
    bins: [1, 3, 1, 3, 4, 5]
  - !Solution
    bins: [2, 3, 1, 3, 4, 5]
  - !Solution
    bins: [1, 1, 2, 3, 4, 5]
  - !Solution
    bins: [2, 1, 2, 3, 4, 5]
  - !Solution
    bins: [3, 1, 2, 3, 4, 5]
  - !Solution
    bins: [1, 2, 2, 3, 4, 5]
  - !Solution
    bins: [2, 2, 2, 3, 4, 5]
  - !Solution
    bins: [3, 2, 2, 3, 4, 5]
  - !Solution
    bins: [1, 3, 2, 3, 4, 5]
  - !Solution
    bins: [2, 3, 2, 3, 4, 5]
  - !Solution
    bins: [1, 1, 3, 3, 4, 5]
  - !Solution
    bins: [2, 1, 3, 3, 4, 5]
  - !Solution
    bins: [1, 2, 3, 3, 4, 5]
  - !Solution
    bins: [2, 2, 3, 3, 4, 5]
options:
  all_solutions: true
***/

include "bin_packing.mzn";

% The example from the GCC.
%
constraint bin_packing(5, [3, 1, 3], [4, 3, 1]);

array[1..6] of var 1..5: bins;

constraint bin_packing(3, bins, [1, 1, 1, 2, 3, 3]);

solve satisfy;

output ["bins = array1d(1..6, ", show(bins), ");\n"];
