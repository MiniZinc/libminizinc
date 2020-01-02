/***
!Test
expected:
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 3..4, !!set {5, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !Range 3..4, !!set {6, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !Range 3..4, !Range 7..8, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {3, 5}, !!set {4, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {3, 5}, !!set {6, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {3, 5}, !Range 7..8, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {3, 6}, !!set {4, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {3, 6}, !!set {5, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {3, 6}, !Range 7..8, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {3, 7}, !!set {4, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {3, 7}, !!set {5, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {3, 7}, !!set {6, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !Range 6..7, !!set {5, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {5, 7}, !!set {6, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !Range 5..6, !Range 7..8, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !Range 6..7, !!set {4, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {4, 7}, !!set {6, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {4, 6}, !Range 7..8, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {5, 7}, !!set {4, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {4, 7}, !!set {5, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !Range 4..5, !Range 7..8, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !Range 5..6, !!set {4, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {4, 6}, !!set {5, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !Range 4..5, !!set {6, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 4..5, !!set {3, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {4, 6}, !!set {3, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {4, 7}, !!set {3, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !Range 5..6, !!set {3, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {5, 7}, !!set {3, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !Range 6..7, !!set {3, 8}, !Range 1..2]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 2..3, !!set {5, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !Range 2..3, !!set {6, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !Range 2..3, !Range 7..8, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 2..3, !!set {4, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !Range 2..3, !!set {6, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !Range 2..3, !Range 7..8, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !Range 2..3, !!set {4, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !Range 2..3, !!set {5, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !Range 2..3, !Range 7..8, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !Range 2..3, !!set {4, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !Range 2..3, !!set {5, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !Range 2..3, !!set {6, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {2, 7}, !!set {5, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {2, 6}, !!set {5, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {2, 7}, !!set {6, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {2, 5}, !!set {6, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {2, 6}, !Range 7..8, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {2, 5}, !Range 7..8, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {2, 7}, !!set {6, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {2, 6}, !Range 7..8, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {2, 7}, !!set {4, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {2, 4}, !Range 7..8, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {2, 6}, !!set {4, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {2, 4}, !!set {6, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {2, 7}, !!set {4, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {2, 5}, !!set {4, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {2, 7}, !!set {5, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {2, 4}, !!set {5, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {2, 5}, !Range 7..8, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {2, 4}, !Range 7..8, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {2, 4}, !!set {6, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {2, 4}, !!set {5, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {2, 5}, !!set {6, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {2, 5}, !!set {4, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {2, 6}, !!set {5, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {2, 6}, !!set {4, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {2, 4}, !!set {3, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {2, 4}, !!set {3, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {2, 4}, !!set {3, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {2, 4}, !!set {5, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {2, 4}, !!set {6, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {2, 4}, !Range 7..8, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {2, 5}, !Range 7..8, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {2, 5}, !!set {3, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {2, 5}, !!set {6, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {2, 5}, !!set {3, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {2, 5}, !!set {4, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {2, 5}, !!set {3, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {2, 6}, !!set {3, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {2, 6}, !!set {3, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {2, 6}, !!set {3, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {2, 6}, !!set {4, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {2, 6}, !!set {5, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {2, 6}, !Range 7..8, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {2, 7}, !!set {6, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {2, 7}, !!set {3, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {2, 7}, !!set {5, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {2, 7}, !!set {3, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {2, 7}, !!set {4, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {2, 7}, !!set {3, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !Range 6..7, !!set {5, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {5, 7}, !!set {6, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !Range 5..6, !Range 7..8, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !Range 6..7, !!set {4, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {4, 7}, !!set {6, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {4, 6}, !Range 7..8, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {5, 7}, !!set {4, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {4, 7}, !!set {5, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !Range 4..5, !Range 7..8, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !Range 5..6, !!set {4, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {4, 6}, !!set {5, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !Range 4..5, !!set {6, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !Range 6..7, !!set {2, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {5, 7}, !!set {2, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !Range 5..6, !!set {2, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {4, 7}, !!set {2, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {4, 6}, !!set {2, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 4..5, !!set {2, 8}, !!set {1, 3}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {3, 5}, !!set {2, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {3, 5}, !!set {6, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {3, 5}, !Range 7..8, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {3, 6}, !!set {2, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {3, 6}, !!set {5, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {3, 6}, !Range 7..8, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {3, 7}, !!set {2, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {3, 7}, !!set {5, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {3, 7}, !!set {6, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !Range 5..6, !!set {2, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !Range 5..6, !!set {3, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !Range 5..6, !Range 7..8, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {5, 7}, !!set {2, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {5, 7}, !!set {3, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {5, 7}, !!set {6, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !Range 6..7, !!set {2, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !Range 6..7, !!set {3, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !Range 6..7, !!set {5, 8}, !!set {1, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 3..4, !!set {2, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {3, 6}, !!set {2, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {3, 7}, !!set {2, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {4, 6}, !!set {2, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {4, 7}, !!set {2, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !Range 6..7, !!set {2, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {4, 6}, !!set {3, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {4, 7}, !!set {3, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !Range 6..7, !!set {3, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {3, 6}, !!set {4, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {3, 7}, !!set {4, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !Range 6..7, !!set {4, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !Range 3..4, !!set {6, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {3, 7}, !!set {6, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {4, 7}, !!set {6, 8}, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !Range 3..4, !Range 7..8, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {3, 6}, !Range 7..8, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {4, 6}, !Range 7..8, !!set {1, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {5, 7}, !!set {4, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {4, 7}, !!set {5, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !Range 4..5, !Range 7..8, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {5, 7}, !!set {3, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {3, 7}, !!set {5, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {3, 5}, !Range 7..8, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {4, 7}, !!set {3, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {3, 7}, !!set {4, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !Range 3..4, !Range 7..8, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !Range 4..5, !!set {3, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {3, 5}, !!set {4, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !Range 3..4, !!set {5, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {5, 7}, !!set {2, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {4, 7}, !!set {2, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !Range 4..5, !!set {2, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {3, 7}, !!set {2, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {3, 5}, !!set {2, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !Range 3..4, !!set {2, 8}, !!set {1, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !Range 3..4, !!set {2, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !Range 3..4, !!set {5, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !Range 3..4, !!set {6, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {3, 5}, !!set {2, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {3, 5}, !!set {4, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {3, 5}, !!set {6, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {3, 6}, !!set {2, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {3, 6}, !!set {4, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {3, 6}, !!set {5, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !Range 4..5, !!set {2, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !Range 4..5, !!set {3, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !Range 4..5, !!set {6, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {4, 6}, !!set {2, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {4, 6}, !!set {3, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {4, 6}, !!set {5, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !Range 5..6, !!set {2, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !Range 5..6, !!set {3, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !Range 5..6, !!set {4, 8}, !!set {1, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !Range 6..7, !!set {1, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {5, 7}, !!set {1, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !Range 5..6, !!set {1, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {4, 7}, !!set {1, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {4, 6}, !!set {1, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !Range 4..5, !!set {1, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !Range 6..7, !!set {1, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {5, 7}, !!set {1, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !Range 5..6, !!set {1, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {3, 7}, !!set {1, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {3, 6}, !!set {1, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {3, 5}, !!set {1, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !Range 6..7, !!set {1, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {4, 7}, !!set {1, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {4, 6}, !!set {1, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {3, 7}, !!set {1, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {3, 6}, !!set {1, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !Range 3..4, !!set {1, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {5, 7}, !!set {1, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {4, 7}, !!set {1, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !Range 4..5, !!set {1, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {3, 7}, !!set {1, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {3, 5}, !!set {1, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !Range 3..4, !!set {1, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !Range 5..6, !!set {1, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {4, 6}, !!set {1, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !Range 4..5, !!set {1, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {3, 6}, !!set {1, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {3, 5}, !!set {1, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !Range 3..4, !!set {1, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !Range 6..7, !!set {1, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {5, 7}, !!set {1, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !Range 5..6, !!set {1, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {2, 7}, !!set {1, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {2, 6}, !!set {1, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {2, 5}, !!set {1, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !Range 6..7, !!set {1, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {4, 7}, !!set {1, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {4, 6}, !!set {1, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {2, 7}, !!set {1, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {2, 6}, !!set {1, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {2, 4}, !!set {1, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {5, 7}, !!set {1, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {4, 7}, !!set {1, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !Range 4..5, !!set {1, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {2, 7}, !!set {1, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {2, 5}, !!set {1, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {2, 4}, !!set {1, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !Range 5..6, !!set {1, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {4, 6}, !!set {1, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !Range 4..5, !!set {1, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {2, 6}, !!set {1, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {2, 5}, !!set {1, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {2, 4}, !!set {1, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !Range 6..7, !!set {1, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {3, 7}, !!set {1, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {3, 6}, !!set {1, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {2, 7}, !!set {1, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {2, 6}, !!set {1, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !Range 2..3, !!set {1, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {5, 7}, !!set {1, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {3, 7}, !!set {1, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {3, 5}, !!set {1, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {2, 7}, !!set {1, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {2, 5}, !!set {1, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !Range 2..3, !!set {1, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !Range 5..6, !!set {1, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {3, 6}, !!set {1, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {3, 5}, !!set {1, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {2, 6}, !!set {1, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {2, 5}, !!set {1, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !Range 2..3, !!set {1, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {4, 7}, !!set {1, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {3, 7}, !!set {1, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !Range 3..4, !!set {1, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {2, 7}, !!set {1, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {2, 4}, !!set {1, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !Range 2..3, !!set {1, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {4, 6}, !!set {1, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {3, 6}, !!set {1, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !Range 3..4, !!set {1, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {2, 6}, !!set {1, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {2, 4}, !!set {1, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !Range 2..3, !!set {1, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 4..5, !!set {1, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {3, 5}, !!set {1, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 3..4, !!set {1, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {2, 5}, !!set {1, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {2, 4}, !!set {1, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 2..3, !!set {1, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {1, 4}, !!set {5, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {1, 4}, !!set {6, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {1, 4}, !Range 7..8, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {1, 5}, !!set {4, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {1, 5}, !!set {6, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {1, 5}, !Range 7..8, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {1, 6}, !!set {4, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {1, 6}, !!set {5, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {1, 6}, !Range 7..8, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {1, 7}, !!set {4, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {1, 7}, !!set {5, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {1, 7}, !!set {6, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 4..5, !!set {6, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !Range 4..5, !Range 7..8, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {4, 6}, !!set {5, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {4, 6}, !Range 7..8, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {4, 7}, !!set {5, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {4, 7}, !!set {6, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 5..6, !!set {4, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !Range 5..6, !Range 7..8, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {5, 7}, !!set {4, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {5, 7}, !!set {6, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !Range 6..7, !!set {4, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !Range 6..7, !!set {5, 8}, !Range 2..3]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !Range 6..7, !!set {5, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {5, 7}, !!set {6, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !Range 5..6, !Range 7..8, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !Range 6..7, !!set {3, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {3, 7}, !!set {6, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {3, 6}, !Range 7..8, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {5, 7}, !!set {3, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {3, 7}, !!set {5, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {3, 5}, !Range 7..8, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 5..6, !!set {3, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {3, 6}, !!set {5, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {3, 5}, !!set {6, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {1, 7}, !!set {6, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {1, 6}, !Range 7..8, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {1, 7}, !!set {5, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {1, 5}, !Range 7..8, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {1, 6}, !!set {5, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {1, 5}, !!set {6, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {1, 7}, !!set {3, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {1, 3}, !Range 7..8, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {1, 6}, !!set {3, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {1, 3}, !!set {6, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {1, 5}, !!set {3, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {1, 3}, !!set {5, 8}, !!set {2, 4}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {1, 3}, !!set {4, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {1, 3}, !!set {6, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {1, 3}, !Range 7..8, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {1, 4}, !!set {3, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {1, 4}, !!set {6, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {1, 4}, !Range 7..8, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {1, 6}, !!set {3, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {1, 6}, !!set {4, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {1, 6}, !Range 7..8, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {1, 7}, !!set {3, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {1, 7}, !!set {4, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {1, 7}, !!set {6, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 3..4, !!set {6, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !Range 3..4, !Range 7..8, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {3, 6}, !!set {4, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {3, 6}, !Range 7..8, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {3, 7}, !!set {4, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {3, 7}, !!set {6, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {4, 6}, !!set {3, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {4, 6}, !Range 7..8, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {4, 7}, !!set {3, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {4, 7}, !!set {6, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !Range 6..7, !!set {3, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !Range 6..7, !!set {4, 8}, !!set {2, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {5, 7}, !!set {4, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {4, 7}, !!set {5, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !Range 4..5, !Range 7..8, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {5, 7}, !!set {3, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {3, 7}, !!set {5, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {3, 5}, !Range 7..8, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {4, 7}, !!set {3, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {3, 7}, !!set {4, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !Range 3..4, !Range 7..8, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 4..5, !!set {3, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {3, 5}, !!set {4, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 3..4, !!set {5, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {1, 7}, !!set {5, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {1, 5}, !Range 7..8, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {1, 7}, !!set {4, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {1, 4}, !Range 7..8, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {1, 5}, !!set {4, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {1, 4}, !!set {5, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {1, 7}, !!set {3, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {1, 3}, !Range 7..8, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {1, 5}, !!set {3, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {1, 3}, !!set {5, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {1, 4}, !!set {3, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {1, 3}, !!set {4, 8}, !!set {2, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {1, 4}, !!set {3, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {1, 5}, !!set {3, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {1, 6}, !!set {3, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !Range 4..5, !!set {3, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {4, 6}, !!set {3, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !Range 5..6, !!set {3, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {1, 3}, !!set {4, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {1, 5}, !!set {4, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {1, 6}, !!set {4, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {3, 5}, !!set {4, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {3, 6}, !!set {4, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !Range 5..6, !!set {4, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {1, 3}, !!set {5, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {1, 4}, !!set {5, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {1, 6}, !!set {5, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !Range 3..4, !!set {5, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {3, 6}, !!set {5, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {4, 6}, !!set {5, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {1, 3}, !!set {6, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {1, 4}, !!set {6, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {1, 5}, !!set {6, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !Range 3..4, !!set {6, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {3, 5}, !!set {6, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !Range 4..5, !!set {6, 8}, !!set {2, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !Range 1..2, !Range 7..8, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !Range 1..2, !!set {6, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !Range 1..2, !!set {5, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !Range 1..2, !Range 7..8, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !Range 1..2, !!set {6, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !Range 1..2, !!set {4, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !Range 1..2, !Range 7..8, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !Range 1..2, !!set {5, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !Range 1..2, !!set {4, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !Range 1..2, !!set {6, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !Range 1..2, !!set {5, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !Range 1..2, !!set {4, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !Range 1..2, !Range 7..8, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !Range 1..2, !!set {6, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !Range 1..2, !!set {3, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !Range 1..2, !Range 7..8, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !Range 1..2, !!set {5, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !Range 1..2, !!set {3, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !Range 1..2, !!set {6, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !Range 1..2, !!set {5, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !Range 1..2, !!set {3, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !Range 1..2, !Range 7..8, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !Range 1..2, !!set {4, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !Range 1..2, !!set {3, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !Range 1..2, !!set {6, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !Range 1..2, !!set {4, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !Range 1..2, !!set {3, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 1..2, !!set {5, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 1..2, !!set {4, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !Range 1..2, !!set {3, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {1, 3}, !!set {2, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 3}, !!set {6, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 3}, !Range 7..8, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {1, 3}, !!set {2, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 3}, !!set {5, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 3}, !Range 7..8, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {1, 3}, !!set {2, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 3}, !!set {5, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 3}, !!set {6, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {1, 3}, !!set {2, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 3}, !!set {4, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 3}, !Range 7..8, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {1, 3}, !!set {2, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 3}, !!set {4, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 3}, !!set {6, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {1, 3}, !!set {2, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 3}, !!set {4, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 3}, !!set {5, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 4}, !Range 7..8, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 4}, !!set {6, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 4}, !!set {5, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 4}, !Range 7..8, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 4}, !!set {6, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 4}, !!set {3, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 4}, !Range 7..8, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 4}, !!set {5, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 4}, !!set {3, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 4}, !!set {6, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 4}, !!set {5, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 4}, !!set {3, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {1, 4}, !!set {2, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {1, 4}, !!set {2, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {1, 4}, !!set {2, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {1, 4}, !!set {2, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {1, 4}, !!set {2, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {1, 4}, !!set {2, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 5}, !Range 7..8, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 5}, !!set {6, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 5}, !!set {4, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 5}, !Range 7..8, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 5}, !!set {6, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 5}, !!set {3, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 5}, !Range 7..8, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 5}, !!set {4, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 5}, !!set {3, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 5}, !!set {6, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 5}, !!set {4, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 5}, !!set {3, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {1, 5}, !!set {2, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {1, 5}, !!set {2, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {1, 5}, !!set {2, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {1, 5}, !!set {2, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {1, 5}, !!set {2, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 6..7, !!set {1, 5}, !!set {2, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 6}, !Range 7..8, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 6}, !!set {5, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 6}, !!set {4, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 6}, !Range 7..8, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 6}, !!set {5, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 6}, !!set {3, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 6}, !Range 7..8, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 6}, !!set {4, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 6}, !!set {3, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 6}, !!set {5, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 6}, !!set {4, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 7}, !!set {1, 6}, !!set {3, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {1, 6}, !!set {2, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {1, 6}, !!set {2, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 7}, !!set {1, 6}, !!set {2, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {1, 6}, !!set {2, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 7}, !!set {1, 6}, !!set {2, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {5, 7}, !!set {1, 6}, !!set {2, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 5..6, !!set {1, 7}, !!set {2, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 7}, !!set {5, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 7}, !!set {6, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {4, 6}, !!set {1, 7}, !!set {2, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 7}, !!set {4, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 7}, !!set {6, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 4..5, !!set {1, 7}, !!set {2, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 7}, !!set {4, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 7}, !!set {5, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 6}, !!set {1, 7}, !!set {2, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 6}, !!set {1, 7}, !!set {3, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 7}, !!set {6, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {3, 5}, !!set {1, 7}, !!set {2, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 5}, !!set {1, 7}, !!set {3, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 7}, !!set {5, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 3..4, !!set {1, 7}, !!set {2, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {2, 4}, !!set {1, 7}, !!set {3, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 2..3, !!set {1, 7}, !!set {4, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 6..7, !!set {5, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {5, 7}, !!set {6, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 5..6, !Range 7..8, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !Range 6..7, !!set {2, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {2, 7}, !!set {6, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {2, 6}, !Range 7..8, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {5, 7}, !!set {2, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {2, 7}, !!set {5, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {2, 5}, !Range 7..8, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 5..6, !!set {2, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {2, 6}, !!set {5, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {2, 5}, !!set {6, 8}, !Range 3..4]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {4, 6}, !!set {2, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {4, 7}, !!set {2, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !Range 6..7, !!set {2, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {2, 6}, !!set {4, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {2, 7}, !!set {4, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 6..7, !!set {4, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {2, 4}, !!set {6, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {2, 7}, !!set {6, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {4, 7}, !!set {6, 8}, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {2, 4}, !Range 7..8, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {2, 6}, !Range 7..8, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {4, 6}, !Range 7..8, !!set {3, 5}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {2, 4}, !!set {5, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {2, 4}, !Range 7..8, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {2, 5}, !!set {4, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {2, 5}, !Range 7..8, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {2, 7}, !!set {4, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {2, 7}, !!set {5, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 4..5, !!set {2, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 4..5, !Range 7..8, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {4, 7}, !!set {2, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {4, 7}, !!set {5, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {5, 7}, !!set {2, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {5, 7}, !!set {4, 8}, !!set {3, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !Range 4..5, !!set {2, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {4, 6}, !!set {2, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !Range 5..6, !!set {2, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {2, 5}, !!set {4, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {2, 6}, !!set {4, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 5..6, !!set {4, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {2, 4}, !!set {5, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {2, 6}, !!set {5, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {4, 6}, !!set {5, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {2, 4}, !!set {6, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {2, 5}, !!set {6, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 4..5, !!set {6, 8}, !!set {3, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 6..7, !!set {3, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {3, 7}, !!set {6, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {3, 6}, !Range 7..8, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !Range 6..7, !!set {2, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 7}, !!set {6, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 6}, !Range 7..8, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {3, 7}, !!set {2, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {2, 7}, !!set {3, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !Range 2..3, !Range 7..8, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {3, 6}, !!set {2, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {2, 6}, !!set {3, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 2..3, !!set {6, 8}, !Range 4..5]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 2..3, !!set {5, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !Range 2..3, !Range 7..8, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {2, 5}, !!set {3, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 5}, !Range 7..8, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {2, 7}, !!set {3, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 7}, !!set {5, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {3, 5}, !!set {2, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {3, 5}, !Range 7..8, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {3, 7}, !!set {2, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {3, 7}, !!set {5, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {5, 7}, !!set {2, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {5, 7}, !!set {3, 8}, !!set {4, 6}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {3, 5}, !!set {2, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {3, 6}, !!set {2, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !Range 5..6, !!set {2, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {2, 5}, !!set {3, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {2, 6}, !!set {3, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 5..6, !!set {3, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !Range 2..3, !!set {5, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 6}, !!set {5, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {3, 6}, !!set {5, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !Range 2..3, !!set {6, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 5}, !!set {6, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {3, 5}, !!set {6, 8}, !!set {4, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 3..4, !!set {2, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {3, 7}, !!set {2, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {4, 7}, !!set {2, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !!set {2, 4}, !!set {3, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {2, 7}, !!set {3, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {4, 7}, !!set {3, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 7}, !Range 2..3, !!set {4, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 7}, !!set {4, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {3, 7}, !!set {4, 8}, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !Range 2..3, !Range 7..8, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 4}, !Range 7..8, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 3..4, !Range 7..8, !Range 5..6]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !Range 2..3, !!set {4, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !Range 2..3, !!set {6, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !!set {2, 4}, !!set {3, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 4}, !!set {6, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {2, 6}, !!set {3, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 6}, !!set {4, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 6}, !Range 3..4, !!set {2, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 3..4, !!set {6, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {3, 6}, !!set {2, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {3, 6}, !!set {4, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {4, 6}, !!set {2, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {4, 6}, !!set {3, 8}, !!set {5, 7}]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 4..5, !!set {3, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !!set {3, 5}, !!set {4, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!Range 1..2, !Range 3..4, !!set {5, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !Range 4..5, !!set {2, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 5}, !!set {4, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 3}, !!set {2, 4}, !!set {5, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {3, 5}, !!set {2, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !!set {2, 5}, !!set {3, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 4}, !Range 2..3, !!set {5, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !Range 3..4, !!set {2, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !!set {2, 4}, !!set {3, 8}, !Range 6..7]
- !Result
  solution: !Solution
    alldisj_avsi: [!!set {1, 5}, !Range 2..3, !!set {4, 8}, !Range 6..7]
***/

include "all_disjoint.mzn";

%-----------------------------------------------------------------------------%
% all_disjoint
%-----------------------------------------------------------------------------%
array[1..4] of var set of 1..8: alldisj_avsi :: add_to_output;

constraint forall(i in 1..4) ( card(alldisj_avsi[i]) > 1 );
constraint all_disjoint(alldisj_avsi);
constraint 8 in alldisj_avsi[3];

solve satisfy;
