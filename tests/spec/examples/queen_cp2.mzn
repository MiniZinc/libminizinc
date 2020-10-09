/***
!Test
expected:
- !Result
  solution: !Solution
    q: [2, 4, 6, 8, 3, 1, 7, 5]
- !Result
  solution: !Solution
    q: [2, 5, 7, 1, 3, 8, 6, 4]
- !Result
  solution: !Solution
    q: [2, 5, 7, 4, 1, 8, 6, 3]
- !Result
  solution: !Solution
    q: [2, 6, 1, 7, 4, 8, 3, 5]
- !Result
  solution: !Solution
    q: [2, 6, 8, 3, 1, 4, 7, 5]
- !Result
  solution: !Solution
    q: [2, 7, 3, 6, 8, 5, 1, 4]
- !Result
  solution: !Solution
    q: [2, 7, 5, 8, 1, 4, 6, 3]
- !Result
  solution: !Solution
    q: [2, 8, 6, 1, 3, 5, 7, 4]
***/

% n-queens example in Zinc using CP techniques
% By Reza Rafeh July 2005
% MiniZinc version
% Peter Stuckey September 30 2006

int: n = 8;

array [1..n] of var 1..n: q :: add_to_output;


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
