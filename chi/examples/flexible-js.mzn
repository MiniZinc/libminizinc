int: horizon;                  % 时间范围
set of int: Time = 0..horizon;
enum Task;
enum Machine; 

array[Task,Machine] of int: d; % 每个机器上的持续时间
int: maxd = max([ d[t,m] | t in Task, m in Machine ]);
int: mind = min([ d[t,m] | t in Task, m in Machine ]);

array[Task] of var Time: S;             % 起始时间
array[Task] of var mind..maxd: D;       % 持续时间
array[Task,Machine] of var opt Time: O; % 可选择的任务起始

constraint forall(t in Task)(alternative(S[t],D[t],
                 [O[t,m]|m in Machine],[d[t,m]|m in Machine]));
constraint forall(m in Machine)
                 (disjunctive([O[t,m]|t in Task],[d[t,m]|t in Task]));
constraint cumulative(S,D,[1|i in Task],k);

solve minimize max(t in Task)(S[t] + D[t]);
