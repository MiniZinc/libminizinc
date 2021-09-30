/***
!Test
expected:
- !Result
  solution: !Solution
    x: [!!set {1}, !!set {1}]
- !Result
  solution: !Solution
    x: [!!set {1}, !!set {2}]
***/

% Regression test for accidental removal of decomposition from fzn_seq_precede_chain_set

include "globals.mzn";

array [1..2] of var set of 1..3: x;

constraint forall (x_i in x) (card(x_i) = 1);

constraint seq_precede_chain(x);
