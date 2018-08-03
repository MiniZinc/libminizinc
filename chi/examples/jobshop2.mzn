int: jobs;                                    % 作业的数量
set of int: JOB = 1..jobs;
int: tasks;                                   % 每个作业的任务数量
set of int: TASK = 1..tasks;
array [JOB,TASK] of int: d;                   % 任务持续时间
int: total = sum(i in JOB, j in TASK)(d[i,j]);% 总持续时间
int: digs = ceil(log(10.0,total));            % 输出的数值
array [JOB,TASK] of var 0..total: s;          % 起始时间
var 0..total: end;                            % 总结束时间

% nooverlap
predicate no_overlap(var int:s1, int:d1, var int:s2, int:d2) =
    s1 + d1 <= s2 \/ s2 + d2 <= s1;

constraint %% 保证任务按照顺序出现
    forall(i in JOB) (
        forall(j in 1..tasks-1) 
            (s[i,j] + d[i,j] <= s[i,j+1]) /\
        s[i,tasks] + d[i,tasks] <= end
    );

constraint %% 保证任务之间没有重叠
    forall(j in TASK) (
        forall(i,k in JOB where i < k) (
            no_overlap(s[i,j], d[i,j], s[k,j], d[k,j])
        )
    );

solve minimize end;

output ["end = \(end)\n"] ++
       [ show_int(digs,s[i,j]) ++ " " ++ 
         if j == tasks then "\n" else "" endif |
         i in JOB, j in TASK ];
