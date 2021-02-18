/***
!Test
expected:
- !Result
  solution: !Solution
    x:
    - 3
    - 2
    - 1
- !Result
  solution: !Solution
    x:
    - 2
    - 4
    - 1
- !Result
  solution: !Solution
    x:
    - 1
    - 2
    - 4
- !Result
  solution: !Solution
    x:
    - 1
    - 3
    - 2
- !Result
  solution: !Solution
    x:
    - 2
    - 3
    - 4
- !Result
  solution: !Solution
    x:
    - 4
    - 1
    - 3
- !Result
  solution: !Solution
    x:
    - 4
    - 3
    - 2
- !Result
  solution: !Solution
    x:
    - 3
    - 4
    - 1
- !Result
  solution: !Solution
    x:
    - 1
    - 2
    - 3
- !Result
  solution: !Solution
    x:
    - 3
    - 1
    - 4
***/

% Regression test for bug #70: set2array coercions were not supported.

include "global_cardinality_low_up.mzn";

predicate myall_different(array[int] of var int: x) =
    global_cardinality_low_up(
        x, set2array(dom_array(x)), [0 | i in dom_array(x)], [1 | i in dom_array(x)]);

array[1..3] of var 1..4: x;

constraint myall_different(x);

solve :: int_search(x, input_order, indomain_min, complete) satisfy;

output ["x = ", show(x), ";\n"];
