% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

% n-queens example in Zinc using CP techniques
% By Reza Rafeh July 2005
% MiniZinc version
% Peter Stuckey September 30 2006

int: n = 8;

array [1..n] of var 1..n: q;


constraint
    alldifferent(q)     % rows
    /\ 
    alldifferent(i in 1..n)(q[i] + i-1)     % diagonals
    /\
    alldifferent(i in 1..n)(q[i] + n-i);

constraint q[1] = 2;
%constraint q[2] = 1;

include "alldifferent.mzn";


solve ::
	int_search(
		q,
		first_fail,
		indomain_min,
		complete
	)
	satisfy;

output	["8 queens, CP version:\n"] ++
	[	if fix(q[i]) = j then "Q " else ". " endif ++
	 	if j = n then "\n" else "" endif
	|	i, j in 1..n
	];
