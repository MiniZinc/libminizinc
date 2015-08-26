% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear

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


% ----------------------------------------------------------
% SEARCH 
% ----------------------------------------------------------

include "minisearch.mzn";
int: k = 4; % find 4 solutions

solve ::
	int_search(
		q,
		first_fail,
		indomain_min,
		complete
	)
search 
find_k_solutions(k);


function ann: find_k_solutions(int: k) = 
   repeat (i in 1..k) (
      if next() then print("Solution "++show(i)++":\n") /\ print() 
      else print("No more solutions found\n") /\ break endif
   ) 
;


output	["8 queens, CP version:\n"] ++
	[	%show(fix(q[i])) ++ ", "
                if fix(q[i]) == j %fix(q[i]) = j
                   then "Q " else ". " endif ++
	 	if j = n then "\n" else "" endif
	|	i, j in 1..n
	]
;
