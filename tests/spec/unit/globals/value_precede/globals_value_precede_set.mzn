/***
!Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !SolutionSet
  - !Solution
    x:
    - !Range 1..3
    - !Range 1..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {1}
    - !Range 1..3
    - !Range 1..3
  - !Solution
    x:
    - !Range 2..3
    - !Range 1..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {3}
    - !Range 1..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {}
    - !Range 1..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..2
    - !Range 1..3
  - !Solution
    x:
    - !!set {3}
    - !Range 1..2
    - !Range 1..3
  - !Solution
    x:
    - !Range 1..3
    - !!set {1, 3}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1, 3}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 3}
    - !Range 1..3
  - !Solution
    x:
    - !Range 2..3
    - !!set {1, 3}
    - !Range 1..3
  - !Solution
    x:
    - !!set {3}
    - !!set {1, 3}
    - !Range 1..3
  - !Solution
    x:
    - !!set {}
    - !!set {1, 3}
    - !Range 1..3
  - !Solution
    x:
    - !Range 1..3
    - !!set {1}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1}
    - !!set {1}
    - !Range 1..3
  - !Solution
    x:
    - !Range 2..3
    - !!set {1}
    - !Range 1..3
  - !Solution
    x:
    - !!set {3}
    - !!set {1}
    - !Range 1..3
  - !Solution
    x:
    - !!set {}
    - !!set {1}
    - !Range 1..3
  - !Solution
    x:
    - !Range 1..3
    - !Range 2..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 2..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {1}
    - !Range 2..3
    - !Range 1..3
  - !Solution
    x:
    - !Range 2..3
    - !Range 2..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {3}
    - !Range 2..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {}
    - !Range 2..3
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {2}
    - !Range 1..3
  - !Solution
    x:
    - !!set {3}
    - !!set {2}
    - !Range 1..3
  - !Solution
    x:
    - !Range 1..3
    - !!set {3}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {3}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1}
    - !!set {3}
    - !Range 1..3
  - !Solution
    x:
    - !Range 2..3
    - !!set {3}
    - !Range 1..3
  - !Solution
    x:
    - !!set {3}
    - !!set {3}
    - !Range 1..3
  - !Solution
    x:
    - !!set {}
    - !!set {3}
    - !Range 1..3
  - !Solution
    x:
    - !Range 1..3
    - !!set {}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1}
    - !!set {}
    - !Range 1..3
  - !Solution
    x:
    - !Range 2..3
    - !!set {}
    - !Range 1..3
  - !Solution
    x:
    - !!set {3}
    - !!set {}
    - !Range 1..3
  - !Solution
    x:
    - !!set {}
    - !!set {}
    - !Range 1..3
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..3
    - !Range 1..2
  - !Solution
    x:
    - !!set {3}
    - !Range 1..3
    - !Range 1..2
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..2
    - !Range 1..2
  - !Solution
    x:
    - !!set {3}
    - !Range 1..2
    - !Range 1..2
  - !Solution
    x:
    - !Range 1..3
    - !!set {1, 3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1, 3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 3}
    - !Range 1..2
  - !Solution
    x:
    - !Range 2..3
    - !!set {1, 3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {3}
    - !!set {1, 3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {}
    - !!set {1, 3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1}
    - !Range 1..2
  - !Solution
    x:
    - !!set {3}
    - !!set {1}
    - !Range 1..2
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 2..3
    - !Range 1..2
  - !Solution
    x:
    - !!set {3}
    - !Range 2..3
    - !Range 1..2
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {2}
    - !Range 1..2
  - !Solution
    x:
    - !!set {3}
    - !!set {2}
    - !Range 1..2
  - !Solution
    x:
    - !Range 1..3
    - !!set {3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {1}
    - !!set {3}
    - !Range 1..2
  - !Solution
    x:
    - !Range 2..3
    - !!set {3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {3}
    - !!set {3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {}
    - !!set {3}
    - !Range 1..2
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {}
    - !Range 1..2
  - !Solution
    x:
    - !!set {3}
    - !!set {}
    - !Range 1..2
  - !Solution
    x:
    - !Range 1..3
    - !Range 1..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1}
    - !Range 1..3
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 2..3
    - !Range 1..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {}
    - !Range 1..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..2
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..2
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 1..3
    - !!set {1, 3}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1, 3}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 3}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 2..3
    - !!set {1, 3}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {3}
    - !!set {1, 3}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {}
    - !!set {1, 3}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 1..3
    - !!set {1}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1}
    - !!set {1}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 2..3
    - !!set {1}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {3}
    - !!set {1}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {}
    - !!set {1}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 1..3
    - !Range 2..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 2..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1}
    - !Range 2..3
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 2..3
    - !Range 2..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {3}
    - !Range 2..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {}
    - !Range 2..3
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {2}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {3}
    - !!set {2}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 1..3
    - !!set {3}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {3}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1}
    - !!set {3}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 2..3
    - !!set {3}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {3}
    - !!set {3}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {}
    - !!set {3}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 1..3
    - !!set {}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {1}
    - !!set {}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 2..3
    - !!set {}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {3}
    - !!set {}
    - !!set {1, 3}
  - !Solution
    x:
    - !!set {}
    - !!set {}
    - !!set {1, 3}
  - !Solution
    x:
    - !Range 1..3
    - !Range 1..3
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..3
    - !!set {1}
  - !Solution
    x:
    - !!set {1}
    - !Range 1..3
    - !!set {1}
  - !Solution
    x:
    - !Range 2..3
    - !Range 1..3
    - !!set {1}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..3
    - !!set {1}
  - !Solution
    x:
    - !!set {}
    - !Range 1..3
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..2
    - !!set {1}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..2
    - !!set {1}
  - !Solution
    x:
    - !Range 1..3
    - !!set {1, 3}
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1, 3}
    - !!set {1}
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 3}
    - !!set {1}
  - !Solution
    x:
    - !Range 2..3
    - !!set {1, 3}
    - !!set {1}
  - !Solution
    x:
    - !!set {3}
    - !!set {1, 3}
    - !!set {1}
  - !Solution
    x:
    - !!set {}
    - !!set {1, 3}
    - !!set {1}
  - !Solution
    x:
    - !Range 1..3
    - !!set {1}
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1}
    - !!set {1}
  - !Solution
    x:
    - !!set {1}
    - !!set {1}
    - !!set {1}
  - !Solution
    x:
    - !Range 2..3
    - !!set {1}
    - !!set {1}
  - !Solution
    x:
    - !!set {3}
    - !!set {1}
    - !!set {1}
  - !Solution
    x:
    - !!set {}
    - !!set {1}
    - !!set {1}
  - !Solution
    x:
    - !Range 1..3
    - !Range 2..3
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 2..3
    - !!set {1}
  - !Solution
    x:
    - !!set {1}
    - !Range 2..3
    - !!set {1}
  - !Solution
    x:
    - !Range 2..3
    - !Range 2..3
    - !!set {1}
  - !Solution
    x:
    - !!set {3}
    - !Range 2..3
    - !!set {1}
  - !Solution
    x:
    - !!set {}
    - !Range 2..3
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {2}
    - !!set {1}
  - !Solution
    x:
    - !!set {3}
    - !!set {2}
    - !!set {1}
  - !Solution
    x:
    - !Range 1..3
    - !!set {3}
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {3}
    - !!set {1}
  - !Solution
    x:
    - !!set {1}
    - !!set {3}
    - !!set {1}
  - !Solution
    x:
    - !Range 2..3
    - !!set {3}
    - !!set {1}
  - !Solution
    x:
    - !!set {3}
    - !!set {3}
    - !!set {1}
  - !Solution
    x:
    - !!set {}
    - !!set {3}
    - !!set {1}
  - !Solution
    x:
    - !Range 1..3
    - !!set {}
    - !!set {1}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {}
    - !!set {1}
  - !Solution
    x:
    - !!set {1}
    - !!set {}
    - !!set {1}
  - !Solution
    x:
    - !Range 2..3
    - !!set {}
    - !!set {1}
  - !Solution
    x:
    - !!set {3}
    - !!set {}
    - !!set {1}
  - !Solution
    x:
    - !!set {}
    - !!set {}
    - !!set {1}
  - !Solution
    x:
    - !Range 1..3
    - !Range 1..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {1}
    - !Range 1..3
    - !Range 2..3
  - !Solution
    x:
    - !Range 2..3
    - !Range 1..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {3}
    - !Range 1..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {}
    - !Range 1..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..2
    - !Range 2..3
  - !Solution
    x:
    - !!set {3}
    - !Range 1..2
    - !Range 2..3
  - !Solution
    x:
    - !Range 1..3
    - !!set {1, 3}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1, 3}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 3}
    - !Range 2..3
  - !Solution
    x:
    - !Range 2..3
    - !!set {1, 3}
    - !Range 2..3
  - !Solution
    x:
    - !!set {3}
    - !!set {1, 3}
    - !Range 2..3
  - !Solution
    x:
    - !!set {}
    - !!set {1, 3}
    - !Range 2..3
  - !Solution
    x:
    - !Range 1..3
    - !!set {1}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1}
    - !!set {1}
    - !Range 2..3
  - !Solution
    x:
    - !Range 2..3
    - !!set {1}
    - !Range 2..3
  - !Solution
    x:
    - !!set {3}
    - !!set {1}
    - !Range 2..3
  - !Solution
    x:
    - !!set {}
    - !!set {1}
    - !Range 2..3
  - !Solution
    x:
    - !Range 1..3
    - !Range 2..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 2..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {1}
    - !Range 2..3
    - !Range 2..3
  - !Solution
    x:
    - !Range 2..3
    - !Range 2..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {3}
    - !Range 2..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {}
    - !Range 2..3
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {2}
    - !Range 2..3
  - !Solution
    x:
    - !!set {3}
    - !!set {2}
    - !Range 2..3
  - !Solution
    x:
    - !Range 1..3
    - !!set {3}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {3}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1}
    - !!set {3}
    - !Range 2..3
  - !Solution
    x:
    - !Range 2..3
    - !!set {3}
    - !Range 2..3
  - !Solution
    x:
    - !!set {3}
    - !!set {3}
    - !Range 2..3
  - !Solution
    x:
    - !!set {}
    - !!set {3}
    - !Range 2..3
  - !Solution
    x:
    - !Range 1..3
    - !!set {}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1}
    - !!set {}
    - !Range 2..3
  - !Solution
    x:
    - !Range 2..3
    - !!set {}
    - !Range 2..3
  - !Solution
    x:
    - !!set {3}
    - !!set {}
    - !Range 2..3
  - !Solution
    x:
    - !!set {}
    - !!set {}
    - !Range 2..3
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..3
    - !!set {2}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..3
    - !!set {2}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..2
    - !!set {2}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..2
    - !!set {2}
  - !Solution
    x:
    - !Range 1..3
    - !!set {1, 3}
    - !!set {2}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1, 3}
    - !!set {2}
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 3}
    - !!set {2}
  - !Solution
    x:
    - !Range 2..3
    - !!set {1, 3}
    - !!set {2}
  - !Solution
    x:
    - !!set {3}
    - !!set {1, 3}
    - !!set {2}
  - !Solution
    x:
    - !!set {}
    - !!set {1, 3}
    - !!set {2}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1}
    - !!set {2}
  - !Solution
    x:
    - !!set {3}
    - !!set {1}
    - !!set {2}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 2..3
    - !!set {2}
  - !Solution
    x:
    - !!set {3}
    - !Range 2..3
    - !!set {2}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {2}
    - !!set {2}
  - !Solution
    x:
    - !!set {3}
    - !!set {2}
    - !!set {2}
  - !Solution
    x:
    - !Range 1..3
    - !!set {3}
    - !!set {2}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {3}
    - !!set {2}
  - !Solution
    x:
    - !!set {1}
    - !!set {3}
    - !!set {2}
  - !Solution
    x:
    - !Range 2..3
    - !!set {3}
    - !!set {2}
  - !Solution
    x:
    - !!set {3}
    - !!set {3}
    - !!set {2}
  - !Solution
    x:
    - !!set {}
    - !!set {3}
    - !!set {2}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {}
    - !!set {2}
  - !Solution
    x:
    - !!set {3}
    - !!set {}
    - !!set {2}
  - !Solution
    x:
    - !Range 1..3
    - !Range 1..3
    - !!set {3}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..3
    - !!set {3}
  - !Solution
    x:
    - !!set {1}
    - !Range 1..3
    - !!set {3}
  - !Solution
    x:
    - !Range 2..3
    - !Range 1..3
    - !!set {3}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..3
    - !!set {3}
  - !Solution
    x:
    - !!set {}
    - !Range 1..3
    - !!set {3}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..2
    - !!set {3}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..2
    - !!set {3}
  - !Solution
    x:
    - !Range 1..3
    - !!set {1, 3}
    - !!set {3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1, 3}
    - !!set {3}
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 3}
    - !!set {3}
  - !Solution
    x:
    - !Range 2..3
    - !!set {1, 3}
    - !!set {3}
  - !Solution
    x:
    - !!set {3}
    - !!set {1, 3}
    - !!set {3}
  - !Solution
    x:
    - !!set {}
    - !!set {1, 3}
    - !!set {3}
  - !Solution
    x:
    - !Range 1..3
    - !!set {1}
    - !!set {3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1}
    - !!set {3}
  - !Solution
    x:
    - !!set {1}
    - !!set {1}
    - !!set {3}
  - !Solution
    x:
    - !Range 2..3
    - !!set {1}
    - !!set {3}
  - !Solution
    x:
    - !!set {3}
    - !!set {1}
    - !!set {3}
  - !Solution
    x:
    - !!set {}
    - !!set {1}
    - !!set {3}
  - !Solution
    x:
    - !Range 1..3
    - !Range 2..3
    - !!set {3}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 2..3
    - !!set {3}
  - !Solution
    x:
    - !!set {1}
    - !Range 2..3
    - !!set {3}
  - !Solution
    x:
    - !Range 2..3
    - !Range 2..3
    - !!set {3}
  - !Solution
    x:
    - !!set {3}
    - !Range 2..3
    - !!set {3}
  - !Solution
    x:
    - !!set {}
    - !Range 2..3
    - !!set {3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {2}
    - !!set {3}
  - !Solution
    x:
    - !!set {3}
    - !!set {2}
    - !!set {3}
  - !Solution
    x:
    - !Range 1..3
    - !!set {3}
    - !!set {3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {3}
    - !!set {3}
  - !Solution
    x:
    - !!set {1}
    - !!set {3}
    - !!set {3}
  - !Solution
    x:
    - !Range 2..3
    - !!set {3}
    - !!set {3}
  - !Solution
    x:
    - !!set {3}
    - !!set {3}
    - !!set {3}
  - !Solution
    x:
    - !!set {}
    - !!set {3}
    - !!set {3}
  - !Solution
    x:
    - !Range 1..3
    - !!set {}
    - !!set {3}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {}
    - !!set {3}
  - !Solution
    x:
    - !!set {1}
    - !!set {}
    - !!set {3}
  - !Solution
    x:
    - !Range 2..3
    - !!set {}
    - !!set {3}
  - !Solution
    x:
    - !!set {3}
    - !!set {}
    - !!set {3}
  - !Solution
    x:
    - !!set {}
    - !!set {}
    - !!set {3}
  - !Solution
    x:
    - !Range 1..3
    - !Range 1..3
    - !!set {}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..3
    - !!set {}
  - !Solution
    x:
    - !!set {1}
    - !Range 1..3
    - !!set {}
  - !Solution
    x:
    - !Range 2..3
    - !Range 1..3
    - !!set {}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..3
    - !!set {}
  - !Solution
    x:
    - !!set {}
    - !Range 1..3
    - !!set {}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 1..2
    - !!set {}
  - !Solution
    x:
    - !!set {3}
    - !Range 1..2
    - !!set {}
  - !Solution
    x:
    - !Range 1..3
    - !!set {1, 3}
    - !!set {}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1, 3}
    - !!set {}
  - !Solution
    x:
    - !!set {1}
    - !!set {1, 3}
    - !!set {}
  - !Solution
    x:
    - !Range 2..3
    - !!set {1, 3}
    - !!set {}
  - !Solution
    x:
    - !!set {3}
    - !!set {1, 3}
    - !!set {}
  - !Solution
    x:
    - !!set {}
    - !!set {1, 3}
    - !!set {}
  - !Solution
    x:
    - !Range 1..3
    - !!set {1}
    - !!set {}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {1}
    - !!set {}
  - !Solution
    x:
    - !!set {1}
    - !!set {1}
    - !!set {}
  - !Solution
    x:
    - !Range 2..3
    - !!set {1}
    - !!set {}
  - !Solution
    x:
    - !!set {3}
    - !!set {1}
    - !!set {}
  - !Solution
    x:
    - !!set {}
    - !!set {1}
    - !!set {}
  - !Solution
    x:
    - !Range 1..3
    - !Range 2..3
    - !!set {}
  - !Solution
    x:
    - !!set {1, 3}
    - !Range 2..3
    - !!set {}
  - !Solution
    x:
    - !!set {1}
    - !Range 2..3
    - !!set {}
  - !Solution
    x:
    - !Range 2..3
    - !Range 2..3
    - !!set {}
  - !Solution
    x:
    - !!set {3}
    - !Range 2..3
    - !!set {}
  - !Solution
    x:
    - !!set {}
    - !Range 2..3
    - !!set {}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {2}
    - !!set {}
  - !Solution
    x:
    - !!set {3}
    - !!set {2}
    - !!set {}
  - !Solution
    x:
    - !Range 1..3
    - !!set {3}
    - !!set {}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {3}
    - !!set {}
  - !Solution
    x:
    - !!set {1}
    - !!set {3}
    - !!set {}
  - !Solution
    x:
    - !Range 2..3
    - !!set {3}
    - !!set {}
  - !Solution
    x:
    - !!set {3}
    - !!set {3}
    - !!set {}
  - !Solution
    x:
    - !!set {}
    - !!set {3}
    - !!set {}
  - !Solution
    x:
    - !Range 1..3
    - !!set {}
    - !!set {}
  - !Solution
    x:
    - !!set {1, 3}
    - !!set {}
    - !!set {}
  - !Solution
    x:
    - !!set {1}
    - !!set {}
    - !!set {}
  - !Solution
    x:
    - !Range 2..3
    - !!set {}
    - !!set {}
  - !Solution
    x:
    - !!set {3}
    - !!set {}
    - !!set {}
  - !Solution
    x:
    - !!set {}
    - !!set {}
    - !!set {}
options:
  all_solutions: true
***/

% A test for the set version of value_precede/3.

include "value_precede.mzn";

array[1..3] of var set of 1..3: x;

constraint value_precede(4, 10, [{4}, {10}, {}]);
constraint value_precede(6, 4, [{}, {1}, {2, 3}]);
constraint value_precede(3, 2, x);
solve satisfy;
output ["x = array1d(1..3, ", show(x), ");\n"];

