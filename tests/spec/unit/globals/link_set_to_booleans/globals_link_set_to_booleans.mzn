/***
!Test
expected:
- !Result
  solution: !Solution
    link_avb:
    - false
    - true
    - false
    - true
    - true
    link_vsi: !!set {1, 4}
***/

include "link_set_to_booleans.mzn";

%-----------------------------------------------------------------------------%
% link_set_to_booleans
%-----------------------------------------------------------------------------%
var set of 0..4: link_vsi ::add_to_output;
array[0..4] of var bool: link_avb ::add_to_output;

constraint link_set_to_booleans(link_vsi,
	array1d(0..4, [false, true, false, false, true]));
constraint link_set_to_booleans({1, 3, 4}, link_avb);

solve satisfy;

output [
    "link_avb = array1d(0..4, ", show(link_avb), ");\n",
    "link_vsi = ", show(link_vsi), ";\n"
];
