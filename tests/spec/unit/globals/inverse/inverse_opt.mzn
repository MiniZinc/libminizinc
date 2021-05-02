/***
!Test
solvers: [gecode]
options: 
  -G: std
  all_solutions: true
expected:
- !Result
  solution: !SolutionSet
  - !Solution
    f: [null, null, null]
    g: [null, null, null, null]
  - !Solution
    f: [2, null, null]
    g: [1, null, null, null]
  - !Solution
    f: [3, null, null]
    g: [null, 1, null, null]
  - !Solution
    f: [4, null, null]
    g: [null, null, 1, null]
  - !Solution
    f: [5, null, null]
    g: [null, null, null, 1]
  - !Solution
    f: [null, 2, null]
    g: [2, null, null, null]
  - !Solution
    f: [null, 3, null]
    g: [null, 2, null, null]
  - !Solution
    f: [null, 4, null]
    g: [null, null, 2, null]
  - !Solution
    f: [null, 5, null]
    g: [null, null, null, 2]
  - !Solution
    f: [3, 2, null]
    g: [2, 1, null, null]
  - !Solution
    f: [2, 3, null]
    g: [1, 2, null, null]
  - !Solution
    f: [4, 2, null]
    g: [2, null, 1, null]
  - !Solution
    f: [2, 4, null]
    g: [1, null, 2, null]
  - !Solution
    f: [4, 3, null]
    g: [null, 2, 1, null]
  - !Solution
    f: [3, 4, null]
    g: [null, 1, 2, null]
  - !Solution
    f: [5, 2, null]
    g: [2, null, null, 1]
  - !Solution
    f: [2, 5, null]
    g: [1, null, null, 2]
  - !Solution
    f: [5, 3, null]
    g: [null, 2, null, 1]
  - !Solution
    f: [3, 5, null]
    g: [null, 1, null, 2]
  - !Solution
    f: [5, 4, null]
    g: [null, null, 2, 1]
  - !Solution
    f: [4, 5, null]
    g: [null, null, 1, 2]
  - !Solution
    f: [null, null, 2]
    g: [3, null, null, null]
  - !Solution
    f: [null, null, 3]
    g: [null, 3, null, null]
  - !Solution
    f: [null, null, 4]
    g: [null, null, 3, null]
  - !Solution
    f: [null, null, 5]
    g: [null, null, null, 3]
  - !Solution
    f: [3, null, 2]
    g: [3, 1, null, null]
  - !Solution
    f: [2, null, 3]
    g: [1, 3, null, null]
  - !Solution
    f: [4, null, 2]
    g: [3, null, 1, null]
  - !Solution
    f: [2, null, 4]
    g: [1, null, 3, null]
  - !Solution
    f: [4, null, 3]
    g: [null, 3, 1, null]
  - !Solution
    f: [3, null, 4]
    g: [null, 1, 3, null]
  - !Solution
    f: [5, null, 2]
    g: [3, null, null, 1]
  - !Solution
    f: [2, null, 5]
    g: [1, null, null, 3]
  - !Solution
    f: [5, null, 3]
    g: [null, 3, null, 1]
  - !Solution
    f: [3, null, 5]
    g: [null, 1, null, 3]
  - !Solution
    f: [5, null, 4]
    g: [null, null, 3, 1]
  - !Solution
    f: [4, null, 5]
    g: [null, null, 1, 3]
  - !Solution
    f: [null, 3, 2]
    g: [3, 2, null, null]
  - !Solution
    f: [null, 2, 3]
    g: [2, 3, null, null]
  - !Solution
    f: [null, 4, 2]
    g: [3, null, 2, null]
  - !Solution
    f: [null, 2, 4]
    g: [2, null, 3, null]
  - !Solution
    f: [null, 4, 3]
    g: [null, 3, 2, null]
  - !Solution
    f: [null, 3, 4]
    g: [null, 2, 3, null]
  - !Solution
    f: [null, 5, 2]
    g: [3, null, null, 2]
  - !Solution
    f: [null, 2, 5]
    g: [2, null, null, 3]
  - !Solution
    f: [null, 5, 3]
    g: [null, 3, null, 2]
  - !Solution
    f: [null, 3, 5]
    g: [null, 2, null, 3]
  - !Solution
    f: [null, 5, 4]
    g: [null, null, 3, 2]
  - !Solution
    f: [null, 4, 5]
    g: [null, null, 2, 3]
  - !Solution
    f: [4, 3, 2]
    g: [3, 2, 1, null]
  - !Solution
    f: [3, 4, 2]
    g: [3, 1, 2, null]
  - !Solution
    f: [4, 2, 3]
    g: [2, 3, 1, null]
  - !Solution
    f: [2, 4, 3]
    g: [1, 3, 2, null]
  - !Solution
    f: [3, 2, 4]
    g: [2, 1, 3, null]
  - !Solution
    f: [2, 3, 4]
    g: [1, 2, 3, null]
  - !Solution
    f: [5, 3, 2]
    g: [3, 2, null, 1]
  - !Solution
    f: [3, 5, 2]
    g: [3, 1, null, 2]
  - !Solution
    f: [5, 2, 3]
    g: [2, 3, null, 1]
  - !Solution
    f: [2, 5, 3]
    g: [1, 3, null, 2]
  - !Solution
    f: [3, 2, 5]
    g: [2, 1, null, 3]
  - !Solution
    f: [2, 3, 5]
    g: [1, 2, null, 3]
  - !Solution
    f: [5, 4, 2]
    g: [3, null, 2, 1]
  - !Solution
    f: [4, 5, 2]
    g: [3, null, 1, 2]
  - !Solution
    f: [5, 2, 4]
    g: [2, null, 3, 1]
  - !Solution
    f: [2, 5, 4]
    g: [1, null, 3, 2]
  - !Solution
    f: [4, 2, 5]
    g: [2, null, 1, 3]
  - !Solution
    f: [2, 4, 5]
    g: [1, null, 2, 3]
  - !Solution
    f: [5, 4, 3]
    g: [null, 3, 2, 1]
  - !Solution
    f: [4, 5, 3]
    g: [null, 3, 1, 2]
  - !Solution
    f: [5, 3, 4]
    g: [null, 2, 3, 1]
  - !Solution
    f: [3, 5, 4]
    g: [null, 1, 3, 2]
  - !Solution
    f: [4, 3, 5]
    g: [null, 2, 1, 3]
  - !Solution
    f: [3, 4, 5]
    g: [null, 1, 2, 3]
***/

include "inverse.mzn";

array [1..3] of var opt 2..5: f;
array [2..5] of var opt 1..3: g;

constraint inverse(f, g);

output ["f = ", concat (i in index_set(f)) (
            if fix(occurs(f[i])) then "(\(i), \(f[i])), " else "" endif
        ), "\n",
        "g = ", concat (i in index_set(g)) (
            if fix(occurs(g[i])) then "(\(i), \(g[i])), " else "" endif
        ), "\n"];
