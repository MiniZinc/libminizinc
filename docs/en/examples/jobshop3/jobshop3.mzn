include "disjunctive.mzn";

enum JOB;
enum TASK;
TASK: last = max(TASK);
array [JOB,TASK] of int: d;                   % task durations
int: total = sum(i in JOB, j in TASK)(d[i,j]);% total duration
int: digs = ceil(log(10.0,total));            % digits for output
array [JOB,TASK] of var 0..total: s;          % start times
var 0..total: end;                            % total end time

constraint %% ensure the tasks occur in sequence
    forall(i in JOB) (
        forall(j in TASK where j < last) 
            (s[i,j] + d[i,j] <= s[i,enum_next(j)]) /\
        s[i,last] + d[i,last] <= end
    );

constraint %% ensure no overlap of tasks
    forall(j in TASK) (
        disjunctive([s[i,j] | i in JOB], [d[i,j] | i in JOB])
    );

solve minimize end;

output ["end = \(end)\n"] ++
       [ show_int(digs,s[i,j]) ++ " " ++ 
         if j == last then "\n" else "" endif |
         i in JOB, j in TASK ];
