/***
!Test
expected:
- !Result
  solution: !Solution
    partitions:
    - !!set {1, 2, 3}
    - !!set {1, 2, 3}
    - !!set {1, 2, 3}
- !Result
  solution: !Solution
    partitions:
    - !Range 1..3
    - !Range 1..3
    - !Range 1..3
- !Result
  solution: !Solution
    partitions:
    - !!set {}
    - !!set {}
    - !!set {}
***/

% Regression test for a bug in mzn2fzn version 1.2 (and up to
% r13389 on the trunk).  The following model used to result in
% variable declaration with a self-assignment being produced
% by mzn2fzn.

array[1..3] of var set of {1,2,3}: partitions :: add_to_output;

constraint array_union(partitions) == array_union(partitions);

solve satisfy;

