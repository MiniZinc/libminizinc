include "disjunctive.mzn";

% Número de trabajos.
int: jobs;
set of int: JOB = 1..jobs;

% Número de tareas por trabajo.
int: tasks;
set of int: TASK = 1..tasks;

% Duración de tareas.
array [JOB,TASK] of int: d;

% Duración total.
int: total = sum(i in JOB, j in TASK)(d[i,j]);

% Dígitos para salida.
int: digs = ceil(log(10.0,total));

% Horas de inicio.
array [JOB,TASK] of var 0..total: s;

% Tiempo de finalización total.
var 0..total: end;

% Asegurar de que las tareas se realicen en secuencia.
constraint %% ensure the tasks occur in sequence
    forall(i in JOB) (
        forall(j in 1..tasks-1)
            (s[i,j] + d[i,j] <= s[i,j+1]) /\
        s[i,tasks] + d[i,tasks] <= end
    );

% Garantizar que no se superponen las tareas.
constraint
    forall(j in TASK) (
        disjunctive([s[i,j] | i in JOB], [d[i,j] | i in JOB])
    );

solve minimize end;

output ["Fin = \(end)\n"] ++
       [ show_int(digs,s[i,j]) ++ " " ++
         if j == tasks then "\n" else "" endif |
         i in JOB, j in TASK ];
