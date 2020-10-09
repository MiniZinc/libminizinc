/***
--- !Test
solvers: [gecode, chuffed]
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    Beamtime: 21
    K: 7
    N: [2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    Q:
    - - [0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    - - [1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [2, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    - - [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    - - [0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [2, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    - - [0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [2, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      - [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    objective: 175
--- !Test
solvers: [cbc]
check_against: [gecode]
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution {}
***/

%-----------------------------------------------------------------------------%
% Radiation problem, Minizinc version
%
% Sebastian Brand
% 05/2007
%-----------------------------------------------------------------------------%

%-----------------------------------------------------------------------------%
% Instances
%-----------------------------------------------------------------------------%

m = 5;  % Rows
n = 5;  % Columns

Intensity = [|	% g5c
	 7,  2, 14,  8,  9 |
	13,  4,  1,  2,  9 |
	 5, 12,  2, 11,  9 |
	10,  2,  4,  9,  7 |
	10,  2,  8, 11,  1 |];

%-----------------------------------------------------------------------------%
% Parameters
%-----------------------------------------------------------------------------%

int: m;  % Rows
int: n;  % Columns

set of int: Rows    = 1..m;
set of int: Columns = 1..n;

	% Intensity matrix
array[Rows, Columns] of int: Intensity;


set of int: BTimes = 1..Bt_max;

int: Bt_max   = max(i in Rows, j in Columns) (Intensity[i,j]);
int: Ints_sum = sum(i in Rows, j in Columns) (Intensity[i,j]);

%-----------------------------------------------------------------------------%
% Variables
%-----------------------------------------------------------------------------%

	% Total beam-on time
var 0..Ints_sum: Beamtime;

	% Number of shape matrices
var 0..m*n: K;

	% N[b] is the number of shape matrices with associated beam-on time b
array[BTimes] of var 0..m*n: N;

	% Q[i,j,b] is the number of shape matrices with associated beam-on time
	% b that expose cell (i,j)
array[Rows, Columns, BTimes] of var 0..m*n: Q;
%-----------------------------------------------------------------------------%
% Constraints
%-----------------------------------------------------------------------------%

	% For FD/LP hybrid solving, all these should go the LP solver
	% (with a suitable linearisation of the 'max' expressions).
constraint
	Beamtime = sum(b in BTimes) (b * N[b])
	/\
	K = sum(b in BTimes) (N[b])
	/\
	forall(i in Rows, j in Columns)
		( Intensity[i,j] = sum([b * Q[i,j,b] | b in BTimes]) )
	/\
	forall(i in Rows, b in BTimes)
		( upper_bound_on_increments(N[b], [Q[i,j,b] | j in Columns]) );


predicate upper_bound_on_increments(var int: N_b, array[int] of var int: L) =
	N_b >= L[1] + sum([ max(L[j] - L[j-1], 0) | j in 2..n ]);

%-----------------------------------------------------------------------------%
% Objective
%-----------------------------------------------------------------------------%

solve :: int_search(
		[Beamtime] ++ N ++
		[ Q[i,j,b] | i in Rows, j in Columns, b in BTimes ],
		input_order, indomain_split, complete)
	minimize (ub(K) + 1) * Beamtime + K;
		% really: (Beamtime, K),  i.e. in lexicographic order

%-----------------------------------------------------------------------------%

output ["radiation:\n",
	"B / K = ", show(Beamtime), " / ", show(K), "\n",
	% "N = ", show(N), "\n"
];

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
