/***
!Test
expected: !Result
  solution: !Solution
    l: 1
    u: 2
    d: !Range 1..2
***/

% lb/ub/dom_array should ignore <>
int: l :: output = lb_array([1, 2, <>]);
int: u :: output = ub_array([1, 2, <>]);
set of int: d :: output = dom_array([1, 2, <>]);
