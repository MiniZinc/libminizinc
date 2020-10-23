include "globals.mzn";

int: horizon;                  % time horizon
set of int: Time = 0..horizon;
enum Task;
enum Machine; 

array[Task,Machine] of int: d; % duration on each machine
int: maxd = max([ d[t,m] | t in Task, m in Machine ]);
int: mind = min([ d[t,m] | t in Task, m in Machine ]);

array[Task] of var Time: S;             % start time
array[Task] of var mind..maxd: D;       % duration
array[Task,Machine] of var opt Time: O; % optional task start

constraint forall(t in Task)(alternative(S[t],D[t],
                 [O[t,m]|m in Machine],[d[t,m]|m in Machine]));
constraint forall(m in Machine)
                 (disjunctive([O[t,m]|t in Task],[d[t,m]|t in Task]));
constraint cumulative(S,D,[1|i in Task],card(Machine));

solve minimize max(t in Task)(S[t] + D[t]);
