/***
!Test
expected:
- !Result
  solution: !Solution
    chan_avi: [1, 2, 3, 5, 4, 2, 1, 3, 2]
    chan_avsi:
    - !!set {1, 7}
    - !!set {2, 6, 9}
    - !!set {3, 8}
    - !!set {5}
    - !!set {4}
***/

include "int_set_channel.mzn";

%-----------------------------------------------------------------------------%
% int_set_channel
%-----------------------------------------------------------------------------%
array[1..9] of var int: chan_avi :: add_to_output = [_,_,3,_,4,2,_,3,_];
array[1..5] of var set of 1..20: chan_avsi :: add_to_output =
	[{1,7},{2,6,9},{3,8},{5},{4}];

constraint int_set_channel(chan_avi, chan_avsi);

solve satisfy;

output [
	"chan_avi = array1d(1..9, ", show(chan_avi), ");\n",
	"chan_avsi = array1d(1..5, ", show(chan_avsi), ");\n"
];
