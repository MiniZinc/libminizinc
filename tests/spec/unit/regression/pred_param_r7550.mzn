/***
!Test
solvers: [gecode, chuffed]
expected:
- !Result
  solution: !Solution
    end: 53
    objective: 53
    s: [0, 0, 0, 4, 7, 7, 6, 8, 12, 8, 6, 4, 14, 19, 12, 12, 13, 18, 8, 11, 14, 18, 19, 4, 16, 10, 20, 9, 14, 6, 24, 26, 21, 16, 9, 21, 24, 28, 31, 22, 24, 11, 25, 16, 30, 26, 38, 25, 25, 30, 33, 36, 39, 34, 33, 47, 25, 44, 50, 38]
***/

% The following is a regression test for a flattening bug with predicate
% parameter expansion.  See the log message for r7550.
% It is derived from g12/zinc/benchmarks/minizinc/rcpsp/rcpsp.mzn with 02.dzn

%-----------------------------------------------------------------------------%

include "globals.mzn";

%-----------------------------------------------------------------------------%
% Model parameters.
%

% Resource parameters
%
int: n_res;                                    % The number of resources
array [ 1..n_res ] of int: rc;                 % The resource capacities

% Task parameters
%
int: n_tasks;                                  % The number of tasks
array [ 1..n_tasks ] of int: d;                % The task durations
int: sum_d = sum( i in 1..n_tasks ) (d[i]);    % The total duration
array [ 1..n_res, 1..n_tasks ] of int: rr;     % The resource requirements
array [ 1..n_tasks ] of set of int: suc;       % The task successors

n_res = 4;
rc = [ 18, 22, 20, 14 ];
n_tasks = 60;
d = [ 4, 7, 9, 2, 6, 5, 2, 7, 4, 3, 4, 10, 4, 5, 8, 2, 7, 10, 8, 8, 8, 1, 5, 5, 7, 2, 1, 8, 10, 1, 1, 4, 10, 9, 3, 3, 7, 10, 6, 6, 9, 9, 1, 10, 2, 10, 6, 8, 9, 2, 6, 2, 8, 3, 4, 3, 9, 3, 3, 7 ];
rr = [| 1, 0, 0, 5, 0, 4, 9, 0, 0, 0, 0, 0, 0, 9, 0, 0, 1, 3, 8, 6, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 5, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 5, 0, 0, 9, 0, 1, 6, 0
      | 0, 0, 3, 0, 1, 0, 0, 3, 2, 0, 0, 3, 10, 0, 0, 8, 0, 0, 0, 0, 0, 3, 0, 3, 6, 0, 0, 0, 0, 0, 0, 0, 4, 0, 7, 0, 0, 3, 2, 0, 0, 4, 0, 0, 0, 0, 0, 9, 0, 0, 9, 4, 0, 0, 0, 0, 6, 0, 0, 0
      | 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 4, 1, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 9, 5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 9
      | 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 2, 4, 0, 0, 9, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 7, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0 |];
suc = [ { 4, 12, 24 },
       { 5, 6, 19 },
       { 35 },
       { 7, 11, 30 },
       { 14, 17, 52 },
       { 9, 15, 16 },
       { 8, 10 },
       { 34 },
       { 44 },
       { 20, 31, 42 },
       { 26 },
       { 13, 21, 29 },
       { 18, 22 },
       { 32, 49 },
       { 40, 46, 55 },
       { 46 },
       { 27, 41 },
       { 38 },
       { 25, 60 },
       { 23 },
       { 31 },
       { 48 },
       { 38, 39, 54 },
       { 28 },
       { 37 },
       { 58 },
       { 33, 36, 51 },
       { 49 },
       { 43, 57 },
       { 33, 47 },
       { 45, 48 },
       { 50 },
       { 54 },
       { 57 },
       { 50 },
       { 38, 57 },
       { 39 },
       { 47 },
       { 47 },
       { 53 },
       { 51 },
       { 56 },
       { 51, 58 },
       { 46 },
       { 55 },
       { 52 },
       { 58 },
       { 55 },
       { 54 },
       { 56 },
       { 53 },
       { 60 },
       { 56 },
       { 59 },
       { 60 },
       { 59 },
       { 59 },
       {  },
       {  },
       {  } ];

%-----------------------------------------------------------------------------%
% Model variables.
%
% s: start times
% end: total end time (makespan)
%
array [ 1..n_tasks ] of var 0..sum_d: s;
var 0..sum_d: end;


%-----------------------------------------------------------------------------%
% Constraints.
%
% successor constraints
constraint
   forall ( i in 1..n_tasks, j in suc[i] )
   (
         s[i] + d[i] <= s[j]
   );

% cumulative resource constraints
constraint
   forall ( i in 1..n_res )
   (
      cumulative( 
         s,
         d, 
         [ rr[i,j] | j in 1..n_tasks ], 
         rc[i] 
      )
   );

% makespan constraints
constraint
   forall ( i in 1..n_tasks )
   (
      s[i] + d[i] <= end
   );

%-----------------------------------------------------------------------------%
% Objective.
%
solve
   :: int_search( s, smallest, indomain_min, complete )
   minimize end;

output [
   "s = ",
   show( s ),
   ";\n",
   "end = ",
   show( end ),
   ";\n"
];

%-----------------------------------------------------------------------------%
%%% EOF %%%

