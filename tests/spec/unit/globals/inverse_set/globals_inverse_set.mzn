/***
!Test
expected:
- !Result
  solution: !Solution
    invs_f1:
    - !!set {1}
    - !Range 3..4
    - !Range 2..4
    - !Range 1..3
    invs_f2:
    - !!set {1, 4}
    - !Range 3..4
    - !Range 2..4
    - !Range 2..3
- !Result
  solution: !Solution
    invs_f1:
    - !!set {1}
    - !Range 2..4
    - !Range 2..4
    - !Range 1..2
    invs_f2:
    - !!set {1, 4}
    - !Range 2..4
    - !Range 2..3
    - !Range 2..3
- !Result
  solution: !Solution
    invs_f1:
    - !!set {1}
    - !Range 1..4
    - !Range 2..4
    - !!set {1}
    invs_f2:
    - !!set {1, 2, 4}
    - !Range 2..3
    - !Range 2..3
    - !Range 2..3
- !Result
  solution: !Solution
    invs_f1:
    - !!set {1}
    - !!set {4}
    - !Range 2..4
    - !!set {}
    invs_f2:
    - !!set {1}
    - !!set {3}
    - !!set {3}
    - !Range 2..3
- !Result
  solution: !Solution
    invs_f1:
    - !!set {1}
    - !Range 1..4
    - !Range 2..4
    - !Range 1..3
    invs_f2:
    - !!set {1, 2, 4}
    - !Range 2..4
    - !Range 2..4
    - !Range 2..3
***/

include "inverse_set.mzn";

%-----------------------------------------------------------------------------%
% inverse_set
%-----------------------------------------------------------------------------%
array[1..4] of var set of 1..5: invs_f1 :: add_to_output =
	[{1}, _, {2,3,4}, _    ];
array[1..4] of var set of 1..5: invs_f2 :: add_to_output =
	[_,   _, _,       {3,2}];

constraint inverse_set(invs_f1, invs_f2);

solve satisfy;

output [
   "invs_f1 = array1d(1..4, ", show(invs_f1), ");\n",
   "invs_f2 = array1d(1..4, ", show(invs_f2), ");\n"
];
