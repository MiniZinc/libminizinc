/***
!Test
expected: !Error
***/

% This should abort since {6} is not a subset of index_set(x).

include "roots.mzn";

array[1..5] of var 1..10: x;

constraint roots(x, {6}, {1, 2});

solve satisfy;

output ["ERROR\n"];
