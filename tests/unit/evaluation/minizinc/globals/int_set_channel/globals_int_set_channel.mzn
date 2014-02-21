% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON minizinc_cpx

include "int_set_channel.mzn";

%-----------------------------------------------------------------------------%
% int_set_channel
%-----------------------------------------------------------------------------%
array[1..9] of var int: chan_avi = [_,_,3,_,4,2,_,3,_];
array[1..5] of var set of 1..20: chan_avsi =
	[{1,7},{2,6,9},{3,8},{5},{4}];

constraint int_set_channel(chan_avi, chan_avsi);

solve satisfy;

output [
	"chan_avi = array1d(1..9, ", show(chan_avi), ");\n",
	"chan_avsi = array1d(1..5, ", show(chan_avsi), ");\n"
];
