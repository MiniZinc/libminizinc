/***
!Test
expected:
- !Result
  solution: !Solution
    x: [1, 1, 1, 1, 1, 1, 1, 1, 1]
    y:
    - !Range 1..9
    - !!set {}
    - !!set {}
    - !!set {}
    - !!set {}
- !Result
  solution: !Solution
    x: [5, 3, 1, 1, 1, 1, 1, 1, 1]
    y:
    - !Range 3..9
    - !!set {}
    - !!set {2}
    - !!set {}
    - !!set {1}
- !Result
  solution: !Solution
    x: [2, 2, 2, 2, 2, 2, 2, 2, 2]
    y:
    - !!set {}
    - !Range 1..9
    - !!set {}
    - !!set {}
    - !!set {}
- !Result
  solution: !Solution
    x: [5, 5, 5, 5, 5, 5, 5, 5, 5]
    y:
    - !!set {}
    - !!set {}
    - !!set {}
    - !!set {}
    - !Range 1..9
***/

include "int_set_channel.mzn";

% Check that the domains of x and y are being restricted
% to the index sets.
%
array[1..9] of var int: x;
array[1..5] of var set of 1..20: y;

constraint int_set_channel(x, y);

solve satisfy;

output [
	"x = ", show(x), "\n",
	"y = ", show(y), "\n"
];
