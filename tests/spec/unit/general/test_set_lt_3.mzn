/***
--- !Test
expected: !Result
  solution: !Solution
    y: !!set {1, 4}
***/

% Comparison of sets with holes

set of int: x = {1, 2};
var set of {1, 4}: y;

constraint card(y) = 2;
constraint x < y;
