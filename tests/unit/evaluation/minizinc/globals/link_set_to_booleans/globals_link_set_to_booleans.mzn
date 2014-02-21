% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON minizinc_cpx

include "link_set_to_booleans.mzn";

%-----------------------------------------------------------------------------%
% link_set_to_booleans
%-----------------------------------------------------------------------------%
var set of 0..4: link_vsi;
array[0..4] of var bool: link_avb;

constraint link_set_to_booleans(link_vsi,
	array1d(0..4, [false, true, false, false, true]));
constraint link_set_to_booleans({1, 3, 4}, link_avb);

solve satisfy;

output [
    "link_avb = array1d(0..4, ", show(link_avb), ");\n",
    "link_vsi = ", show(link_vsi), ";\n"
];
