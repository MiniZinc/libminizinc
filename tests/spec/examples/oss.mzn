/***
!Test
expected:
- !Result
  solution: !Solution
    makespan: 1168
    objective: 1168
    start:
    - [507, 0, 6]
    - [0, 168, 663]
    - [168, 663, 339]
- !Result
  solution: !Solution
    makespan: 1168
    objective: 1168
    start:
    - [333, 1162, 0]
    - [1000, 0, 657]
    - [0, 657, 333]
- !Result
  solution: !Solution
    makespan: 1168
    objective: 1168
    start:
    - [339, 0, 6]
    - [0, 168, 663]
    - [168, 663, 339]
- !Result
  solution: !Solution
    makespan: 1168
    objective: 1168
    start:
    - [0, 1162, 829]
    - [1000, 505, 0]
    - [829, 0, 505]
- !Result
  solution: !Solution
    makespan: 1168
    objective: 1168
    start:
    - [339, 1162, 0]
    - [1000, 168, 657]
    - [161, 657, 333]
***/

int: Endtime;
int: NMachines;
int: NJobs;

set of int: Machines = 1..NMachines;
set of int: Jobs = 1..NJobs;

array[Machines,Jobs] of int: duration;

array[Machines,Jobs] of var 0..Endtime: start;
var 0..Endtime: makespan;

predicate not_at_same_time(Machines: m1, Jobs: j1, Machines: m2, Jobs: j2) =
			start[m1,j1] + duration[m1,j1] <= start[m2,j2]
		\/	start[m2,j2] + duration[m2,j2] <= start[m1,j1];


constraint
	forall(m in Machines)(
		forall(j1,j2 in Jobs where j1 < j2)(
			not_at_same_time(m,j1,m,j2)
		)
	);

constraint
	forall(j in Jobs)(
		forall(m1,m2 in Machines where m1 < m2)(
			not_at_same_time(m1,j,m2,j)
		)
	);

constraint
	forall(m in Machines)(
		forall(j in Jobs)(
			start[m,j] + duration[m,j] <= makespan
		)
	);

solve minimize makespan;

output [ "oss:\nmakespan = ", show(makespan), "\nstart = ", show(start), "\n" ];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Endtime = 1509;
NMachines = 3;
NJobs = 3;

duration = [|661,6,333|168,489,343|171,505,324|];
