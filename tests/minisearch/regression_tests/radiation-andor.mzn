%-----------------------------------------------------------------------------%
% Radiation problem, Minizinc version
%
% Sebastian Brand
% 05/2007
%-----------------------------------------------------------------------------%

%-----------------------------------------------------------------------------%
% Parameters
%-----------------------------------------------------------------------------%

include "minisearch.mzn";

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
%array[Rows, Columns, BTimes] of var 0..m*n: Q;

%-----------------------------------------------------------------------------%
% Constraints
%-----------------------------------------------------------------------------%

	% For FD/LP hybrid solving, all these should go the LP solver
	% (with a suitable linearisation of the 'max' expressions).
constraint
	Beamtime = sum(b in BTimes) (b * N[b])
	/\
	K = sum(b in BTimes) (N[b]);


predicate upper_bound_on_increments(var int: N_b, array[int] of var int: L) =
	N_b >= L[1] + sum([ max(L[j] - L[j-1], 0) | j in 2..n ]);
	%
	% Good linear version:
	% let { array[min(index_set(L))..max(index_set(L))] of var int: S } in
	% N_b >= L[1] + sum([ S[j] | j in 2..n ]) /\
	% forall([ S[j] >= L[j] - L[j-1] /\ S[j] >= 0 | j in 2..n ]);
	

%-----------------------------------------------------------------------------%
% Objective
%-----------------------------------------------------------------------------%
%var int: obj = (ub(K) + 1) * Beamtime + K; % this formulation will cause an error

var int: obj;
constraint obj = (ub(K) + 1) * Beamtime + K;


int: timeout = 10*1000; % in milliseconds

solve 
    :: int_search(
		[Beamtime] ++ N,
%	    [Q[i,j,b] | i in Rows, j in Columns, b in BTimes ],
		input_order, indomain_split, complete)
%	minimize obj;
		% really: (Beamtime, K),  i.e. in lexicographic order

search time_limit(timeout, and_or_min()) /\ (if hasSol() then print() else skip() endif);


function ann: and_or_min() = 
         repeat (
            if next() then  % find a solution for the master variables
               %print("Master solution found:\n") /\ print() /\ 
               repeat (row in 1..m) (
                  scope(
                     let { 
                         % subproblem variables
                	% Q[i,j,b] is the number of shape matrices with associated beam-on time
                 	% b that expose cell (i,j)
                         array[Columns, BTimes] of var 0..m*n: Q;
                     } in (                                        
                         post(assignMasterVariables() /\ % assign first stage variables
                              subProblemCts(row,Q)) /\
                         if next() then 
                           skip() /\ print("Solution found for row: "++show(row)++"\n")
                         else print("No solution found for row "++show(row)++"\n") /\
                              break 
                         endif                      
                     )
                 )
              ) % end repeat 
              /\ commit() /\  print("We found a solution with value "++show(obj)++".\n") % we found a solution
              /\ post(obj < sol(obj)) 
           else break endif % we find no master solution         
    )
;

predicate assignMasterVariables() = 
  Beamtime = sol(Beamtime) /\
%  trace("Beamtime = "++show(sol(Beamtime))++"\n") /\
  K = sol(K) /\
%  trace("K = "++show(sol(K))++"\n") /\
  forall(i in BTimes) (N[i] = sol(N[i])) % /\  trace("N["++show(i)++" = "++show(sol(N[i]))++"\n")) %/\ 
%  obj = sol(obj)
;

predicate subProblemCts(int: row, array[Columns, BTimes] of var int: Q) =
	forall(j in Columns)
		( Intensity[row,j] = sum([b * Q[j,b] | b in BTimes]))
%                  /\  trace("Intensity["++show(row)++","++show(j)++"] = "++show(Intensity[row,j])++" = sum()\n")  )
	/\
	forall(b in BTimes)
		( upper_bound_on_increments(N[b], [Q[j,b] | j in Columns]) )
;


%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%
% Instance 
%-----------------------------------------------------------------------------%
m = 5;	% rows
n = 5;	% columns

% max intensity = 8
Intensity = [|
6, 4, 5, 3, 4 |
2, 3, 0, 1, 8 |
0, 7, 5, 6, 3 |
5, 2, 3, 1, 2 |
0, 1, 8, 0, 6
|];



%-----------------------------------------------------------------------------%

output [
    show(obj), 
    "\nN = ", show(N), "\n",
    show(Beamtime), "|", show(K)
]
%    "Q = ", show(Q), "\n",

