% RUNS ON mzn20_fd
include "sliding_sum.mzn";

%-----------------------------------------------------------------------------%
% sliding_sum
%-----------------------------------------------------------------------------%

array[1..7] of var -100..100: seq_avi =
	[_,  0,  99,  _,  _,  _,  _];

constraint sliding_sum(0, 0, 3, seq_avi);

% Example from the GCC.
%
constraint sliding_sum(3, 7, 4, [1, 4, 2, 0, 0, 3, 4]);

solve satisfy;

output ["seq_avi = array1d(1..7, ", show(seq_avi), ");\n"];
