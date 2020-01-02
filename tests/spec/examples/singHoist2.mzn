/***
!Test
expected:
- !Result
  solution: !Solution
    Entry:
    - 62
    - 4
    - 18
    - 47
    Period: 25
    Removal:
    - 0
    - 14
    - 43
    - 57
    objective: 25
***/

%------------------------------------------------------------------------------%
% singHoist2.mzn
% Peter Stuckey
% September 30 2006
%------------------------------------------------------------------------------%
%------------------------------------------------------------------------------%
% singHoist2.zinc
% Jakob Puchinger <jakobp@cs.mu.oz.au>
% Wed Jun 21
%------------------------------------------------------------------------------%

%------------------------------------------------------------------------------%
% Minizinc model of one-hoist scheduling
%
% Robert Rodosek and Mark Wallace
% A Generic Model and Hybrid Algorithm for Hoist Scheduling Problems
% in Michael J. Maher and Jean-Francois Puget eds.
% Principles and Practice of Constraint Programming - CP98,
% Springer, Lecture Notes in Computer Science, volume 1520, 1998.


%------------------------------------------------------------------------------%
% constants

int:                                             NumTanks;
int:                                             NumJobs;
array [0..NumTanks, 0..NumTanks]     of int:     Empty;
array [0..NumTanks]                  of int:     Full;
array [1..NumTanks]                  of int:     MinTime;
array [1..NumTanks]                  of int:     MaxTime;


%------------------------------------------------------------------------------%
% decision variables

int: PerMax = sum([MaxTime[i] | i in 1..NumTanks]);

array [0..NumTanks] of var 0..(NumJobs * PerMax): Entry;
array [0..NumTanks] of var 0..(NumJobs * PerMax): Removal;
var 0..PerMax                                   : Period;

%------------------------------------------------------------------------------%
% model

solve minimize Period;

constraint
    forall (Tank in 0..NumTanks) (
	 Removal[Tank] + Full[Tank] = Entry[(Tank + 1) mod (NumTanks+1)]
    );

constraint
	forall (Tank in 1..NumTanks) (
		Entry[Tank] + MinTime[Tank] <= Removal[Tank]
	/\	Entry[Tank] + MaxTime[Tank] >= Removal[Tank]
	);

constraint
	Removal[NumTanks] + Full[NumTanks] <= NumJobs * Period;

% disjoint constraint
constraint
	forall (T1 in 0..NumTanks-1, T2 in T1+1..NumTanks, K in 1..NumJobs-1) (
	    Entry[(T1 + 1) mod (NumTanks+1)] +
	    Empty[(T1+1) mod (NumTanks+1), T2] + K * Period
	    <= Removal[T2]
	\/  Entry[(T2 + 1) mod (NumTanks+1)] +
	    Empty[(T2 + 1) mod (NumTanks+1), T1]
	    <= Removal[T1] + K * Period
	);

constraint
	Removal[0] = 0;

%------------------------------------------------------------------------------%
% Test case.

NumTanks = 3;
NumJobs = 3;
Empty = array2d(0..NumTanks, 0..NumTanks,
        [ 0, 2, 3, 3,
          2, 0, 2, 3,
          3, 2, 0, 2,
          3, 3, 2, 0]);
Full = array1d(0..NumTanks, [4, 4, 4, 5]);
MinTime = [10, 25, 10];
MaxTime = [10, 25, 10];
constraint 0 <= Period /\ Period <= sum([MaxTime[i] | i in 1..NumTanks]);

output [
	"singHoist2:\n",
	"Period = ", show(Period), "\n",
	"Entry[] =   [",
	show(Entry[0]), " ",
	show(Entry[1]), " ",
	show(Entry[2]), " ",
	show(Entry[3]), "]\n",
	"Removal[] = [",
	show(Removal[0]), " ",
	show(Removal[1]), " ",
	show(Removal[2]), " ",
	show(Removal[3]), "]\n"
];
