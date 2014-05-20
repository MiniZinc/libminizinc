% We don't run this as part of the test suite since it takes a while.
% (Changing the search strategy would probably make it faster, but since
% that might mask the bug we are testing for we don't want to do that.)

% Regression test for bug #92: this model caused an abort during evaluation
% on Windows (due to a stack overflow).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% This MiniZinc model was created by Hakan Kjellerstrand, hakank@bonetmail.com
% now shamelessly and probably poorly modified by Therese Keane
% Inclusion of tidal windows

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Parameters (integers)

% number of jobs 
int: n;

% number of machines 
int: m;

% number of spots
int: s;

% number of tasks in each job
int: o;

% max end task time, bit of a fudge doing it this way
int: pmax;

% number of tidal windows
int: twnum;

% minutes per day
int: minsperday = 1440;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Parameters (sets)

% set of jobs 
set of int: J = 1..n;

% set of machines 
set of int: M = 1..m;

% set of tasks
set of int: O = 1..o;

% set of spots
set of int: S = 1..s;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Parameters (arrays)

% processing time of o on a 
array[M, O] of int: p;

% tidal wimdow array
array[1..twnum, 1..2] of int: tw;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Variables (arrays)

% starting time of task o in job j
array[J,O] of var int: x;

% Y[j,a] is 1 if job j is scheduled to be performed with machine a, and 0 if not
array[J,M] of var 0..1: Y;

% W[j,s] is 1 if job j uses spot s and 0 otherwise
array[J,S] of var 0..1: W;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Variables (integers)

% some large constant 
var int: K = (sum(a in M, o in O) (p[a,o]))*n;

% so-called makespan 
var int: z;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Objective

% the objective is to make z as small as possible 
solve :: int_search(
        [x[i,j] | i in J, j in O] ++ 
	  [Y[i,a] | i in J, a in M] ++
        [W[i,b] | i in J, b in S] ++ [K, z],
        first_fail,
        indomain_min,
        complete
        ) minimize z;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Predicates

% No overlapping
predicate no_overlap(var int:s1, var int:d1, int:s2, int:d2, int:minsday) =
    ((s1 + d1) mod minsday <= s2 \/ s1 mod minsday >= s2 + d2);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Constraints

constraint
  % added K as upper limit of z and x
  z >= 0
  /\
  z <= K
  /\
  forall(i in J, a in O) (
       x[i,a] >= 0
       /\
       x[i,a] <= K
  )

  /\
  
  % ensure correct ordering of tasks within a job
  forall (i in J, j in 2..o, a in M) (
  	x[i,j] >= (x[i, j-1] + p[a, j-1]) * Y[i,a]
  )

  /\

  % ensure start of next job is after end of previous job
  forall(i in 2..n, j in J, a in M where i > j) (
      x[i,1] >= (x[j,o] + p[a,o])*Y[j,a]*Y[i,a]  
  )

  /\

  % ensure start of job 4 (landing/offload) does not occur during a tidal window
  forall(i in J, c in 1..twnum, a in M) (
    no_overlap(x[i,4], p[a,4]*Y[i,a], tw[c,1], tw[c,2], minsperday)
  )
  
  /\

  % only one machine can perform a job
  forall(j in 1..n)(sum(a in 1..m) (Y[j,a]) = 1)

  /\

  % only one spot is used per job
  forall(j in 1..n)(sum(b in 1..s) (W[j,b]) = 1)

  /\

  % ensure jobs don't overlap on each spot
  forall(i in 2..n, j in J, a in M, b in S where i > j) (
      x[i,1] >= ((x[j,1] + p[a,1])*Y[j,a])*W[j,b]*W[i,b]
  )

  /\

  % which is the maximum of the completion times of all the jobs 
  forall(j in J, a in M) ( z >= (x[j,o] + p[a,o])*Y[j,a])
;

output [ 
  "Total-Duration: ", show(z), "\n",
  "Number-of-Jobs: ", show(n), "\n",
  "Number-of-Tasks-per-Job: ", show(o), "\n",
  "Number-of-Watercraft/Helos: ", show(m), "\n",
  "Number-of-Spots: ", show(s), "\n",
  "Number-of-TWs: ", show(twnum), "\n",
  "Job/Task-Start-Times: "
] ++
[
  if a = 1 then "\n" else " " endif ++
    show(x[i,a])
  | i in J, a in O
] ++ [ "\nJob-Watercraft/Helo-use: "] ++
[
  if a = 1 then "\n" else " " endif ++
    show(Y[i,a])
  | i in J, a in M
] ++ [ "\nJob-Spot/Dock-use: "] ++
[
  if a = 1 then "\n" else " " endif ++
    show(W[i,a])
  | i in J, a in S
] ++ [ "\nTidal-Windows: "] ++
[
  if b = 1 then "\n" else " " endif ++
    show(tw[i,b])
  | i in 1..twnum, b in 1..2
] ++ [ "\nGiven-Task-Duration-for-Watercraft/Helo: "] ++
[
  if i = 1 then "\n" else " " endif ++
    show(p[a,i])
  | a in M, i in O
] ++ ["\n"]
;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Input data

% n sorties comprising o tasks, m helos/watercraft
n = 18;		% number of jobs
m = 2;		% number of machines/helos
o = 6;		% number of tasks in each job
s = 1;		% number of spots, used for task 1 at the moment

% processing time of task o with machine a
% task times represent land/load, takeoff, transit, land/offload, takeoff, transit
p = array2d(M, O, [
 24, 5, 60, 24, 5, 50,
 20, 8, 60, 20, 8, 50]);
pmax = 50;
%   8, 4, 65,  8, 4, 65,

% Number of tidal windows
twnum = 2;
% start and duration of tidal windows
tw = array2d(1..2, 1..2, [
  100, 100,
  400, 100]);
